#include "abc2transactioncreditor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>


/*
The TYPICAL [non-error] flow goes like this:
    0) Query a view to get all transactions with state=uncreditted, iterate them. Here's one transaction flow:
        a) Verify state is uncreditted (state of creditted means the view gave stale results, but POSSIBLY error/previous-fail (recoverable) condition)
        b) Lock seller profile towards the transaction (cas swap fail means they are doing other stuff (add user to list of locked profiles), cas swap fail that then reveals locked towards ANY (had:the very same) transaction = neighbor/ourself is on it, so back off rest of view query)
        c) Cas swap transaction state from uncreditted to creditted (cas swap fail means neighbor is on it)
        d) Cas swap unlock+credit profile with the transaction amount (cas swap fail means neighbor is on it)

The ERROR/previous-fail cases:
    0) Finding a profile locked towards a transaction (with the transaction in state 'creditted') -- but only after we wait a few hundred ms, and the profile is STILL locked towards that same transaction (it's very typical for it to have been unlocked within those few hundred ms)

Handling the ERROR/previous-fail cases:
    0) TODOreq (decided to launch instead) The only way I can find all of these is with a view/map that emits when it sees "locked towards transaction". I can find some of them by iterating the transactions list (by sheer luck, the seller had another transaction that we are 'later' trying to credit... and we see it locked to some other transaction and then I guess try to do recovery). COME TO THINK OF IT, I don't need to wait a few hundred ms to do the recovery, BUT I SHOULD because otherwise I'd be stopping "transaction creditors" dead in their tracks (until their next cron timeout). Since I am recovery, I won't be "their neighbor" (yet, since recovery runs first?) -- ahh well since I said "yet", then in fact I don't need to wait a few hundred ms. My head hurts <<-- Another way to do this is to add the transaction the profile is locked towards to the top of the regular view query (and let it 'recover' that way (means I have to be ok with finding a profile locked towards a transaction I haven't started attempting to credit TODOreq (one of the two methods))
*/

//TODOreq: finding a transaction in state "creditting" where the seller doesn't have the profile locked means we failed after unlocking+creditting the profile. As error recovery, we simply change the state from creditting to creditted (ofc if the profile is locked, we know where to resume as well)
Abc2TransactionCreditor::Abc2TransactionCreditor(bool verbose, QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
    , m_Verbose(verbose)
    , m_TotalAmountCreditted(0)
    , m_TotalAmountEarned(0)
{ }
void Abc2TransactionCreditor::errorOutput(const string &errorString)
{
    QString errorQString = QString::fromStdString(errorString);
    emit e(errorQString);
}
void Abc2TransactionCreditor::viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    (const_cast<Abc2TransactionCreditor*>(static_cast<const Abc2TransactionCreditor*>(cookie)))->viewQueryCompleteCallback(error, resp);
}
void Abc2TransactionCreditor::viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp)
{
    if(m_Verbose)
        emit o("viewQueryCompleteCallback called (good)");
    if(error != LCB_SUCCESS)
    {
        emit e("httpCompleteCallback has error");
        emit transactionCredittingFinished(false);
        return;
    }

    //TODOreq: don't trust the view query. we still must get the doc (and we have to anyways to get the CAS in order to fuck with it. i can emit(null,null) (which gives us only the key) in the view) and check AGAIN then that the state is 'uncreditted'

    QByteArray jsonResultByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonResultString(jsonResultByteArray);
    m_ViewQueryPropertyTree.clear();
    std::string jsonResultStdString = jsonResultString.toStdString();
    std::istringstream is(jsonResultStdString);
    read_json(is, m_ViewQueryPropertyTree);
    boost::optional<std::string> errorMaybe = m_ViewQueryPropertyTree.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        emit e("json had error set");
        emit transactionCredittingFinished(false);
        return;
    }
    m_ViewQueryHadError = false;
}
bool Abc2TransactionCreditor::creditTransactionIfStateReallyIsUncredittedAndReturnFalseIfWeShouldStopIteratingViewQueryResults(const string &keyToTransactionMaybeWithStateOfUncreditted)
{
    //"maybe" uncreditted because we don't trust the view query. it may have given us stale data

    //check state is really uncreditted
    std::string getDescription = "the transaction for verifying in state uncreditted for the first step of creditting: " + keyToTransactionMaybeWithStateOfUncreditted;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(keyToTransactionMaybeWithStateOfUncreditted, getDescription))
    {
        return false;
    }
    lcb_cas_t transactionDocCas = m_LastGetCas;
    ptree transactionDoc;
    std::istringstream transactionDocIs(m_LastDocGetted);
    read_json(transactionDocIs, transactionDoc);
    const std::string &transactionState = transactionDoc.get<std::string>(JSON_TRANSACTION_STATE_KEY);
    //if(transactionState == JSON_TRANSACTION_STATE_VALUE_CREDITTED)
    if(transactionState != JSON_TRANSACTION_STATE_VALUE_UNCREDITTED)
    {
        //the view query was stale (despite stale=false) and the state was not in fact not done... so we just return true and continue onto the next one
        return true;
    }

    //get seller profile
    const std::string &transactionSeller = transactionDoc.get<std::string>(JSON_TRANSACTION_SELLER);
    QString transactionSellerQString = QString::fromStdString(transactionSeller);
    if(m_UsersWithProfilesFoundLocked.contains(transactionSellerQString))
        return true;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(transactionSeller), "getting transaction seller's account to see if locked before locking into creditting"))
    {
        return false;
    }
    lcb_cas_t sellerAccountDocCas = m_LastGetCas;
    ptree sellerAccountDoc;
    std::istringstream sellerAccountDocIs(m_LastDocGetted);
    read_json(sellerAccountDocIs, sellerAccountDoc);

    //state is 'uncreditted'

     //check seller profile to see if locked (if unlocked, lock)
    if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(sellerAccountDoc))
    {
        m_UsersWithProfilesFoundLocked.insert(transactionSellerQString);
        return true;
    }

    //profile not locked, so lock

    sellerAccountDoc.put(JSON_USER_ACCOUNT_LOCKED_CREDITTING_TRANSACTION, keyToTransactionMaybeWithStateOfUncreditted);
    std::string opDescription = "locking transaction seller user account into creditting transaction: " + keyToTransactionMaybeWithStateOfUncreditted;
    if(!couchbaseStoreRequestWithExponentialBackoff(userAccountKey(transactionSeller), Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(sellerAccountDoc), LCB_SET, sellerAccountDocCas, opDescription)) //TODOreq: check all uses of 'requiring success' are appropriate. if we want to proceed when lcb op fails, DO NOT use the "require success" variant. NOTE: if success isn't seen (in the 'requring success' variant), lcb_t is destroyed!!!
    {
        //db error
        return false;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        if(m_LastOpStatus == LCB_KEY_EEXISTS) //cas-swap fail
        {
            //we "require success", but we don't want to stop the entire app when we don't see it... we just want to not fuck with that user's profile for the rest of this run (hence return TRUE)

            //but first we dig deeper to see if a neighbor/ourself locked the account into creditting ANY transaction, in which case we back off by returning false
            if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(transactionSeller), "getting user profile that just cas swap failed, to see if a neighbor is creditting transactions which means we should back off"))
            {
                return false; //db error
            }
            ptree profileThatJustCasSwapFailed; //yea, it may have UNLOCKED by now rofl, but whatever~
            std::istringstream profileThatJustCasSwapFailedIs;
            read_json(profileThatJustCasSwapFailedIs, profileThatJustCasSwapFailed);
            const std::string &profileLockedCredittingTransactionTEST = profileThatJustCasSwapFailed.get<std::string>(JSON_USER_ACCOUNT_LOCKED_CREDITTING_TRANSACTION, "n");
            if(profileLockedCredittingTransactionTEST != "n")
            {
                return false; //backoff to neighbor
            }

            m_UsersWithProfilesFoundLocked.insert(transactionSellerQString);
            return true;
        }

        //error other than cas swap fail = db error, so return false (which stops us)
        errorOutput("failed while: " + opDescription);
        return false;
    }
    lcb_cas_t sellerAccountLockedToCredittingTransactionCas = m_LastSetCas;

    //profile locked to 'creditting transaction'

    //change transaction state from 'uncreddited' to 'creditted'
    transactionDoc.put(JSON_TRANSACTION_STATE_KEY, JSON_TRANSACTION_STATE_VALUE_CREDITTED);
    opDescription = "changing transaction state from uncreditted to creditted: " + keyToTransactionMaybeWithStateOfUncreditted;
    if(!couchbaseStoreRequestWithExponentialBackoff(keyToTransactionMaybeWithStateOfUncreditted, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(transactionDoc), LCB_SET, transactionDocCas, opDescription))
    {
        //db error
        return false;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        if(m_LastOpStatus == LCB_KEY_EEXISTS) //cas-swap fail
        {
            //only ourselves as a neighbor could possibly have updated the transaction doc to make a cas swap fail, so we 'back off' completely (hence return false)
            return false;
        }

        //error other than cas swap fail = db error, so spit out what we were doing and return false (which stops us)
        errorOutput("failed while: " + opDescription);
        return false;
    }

    //transaction doc in state 'creditted'

    //cas swap+credit+unlock the profile
    SatoshiInt transactionAmountInSatoshis = boost::lexical_cast<SatoshiInt>(transactionDoc.get<std::string>(JSON_TRANSACTION_AMOUNT));
    double transactionAmountInJsonDouble = satoshiIntToJsonDouble(transactionAmountInSatoshis);
    double transactionFeeAmountInJsonDouble = transactionFeeForTransactionAmount(transactionAmountInJsonDouble);
    SatoshiInt transactionFeeInSatoshis = jsonDoubleToSatoshiIntIncludingRounding(transactionFeeAmountInJsonDouble); //the rounding, it does nothing
    if(transactionSellerQString == "d3fault")
    {
        m_TotalAmountEarned += transactionAmountInSatoshis;
    }
    else
    {
        m_TotalAmountEarned += transactionFeeInSatoshis;
    }
    SatoshiInt userBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(sellerAccountDoc.get<std::string>(JSON_USER_ACCOUNT_BALANCE));
    SatoshiInt transactionAmountAfterFeeInSatoshis = (transactionAmountInSatoshis - transactionFeeInSatoshis);
    //userBalanceInSatoshis += transactionAmountInSatoshis;
    userBalanceInSatoshis += transactionAmountAfterFeeInSatoshis;
    m_TotalAmountCreditted += transactionAmountAfterFeeInSatoshis;
    sellerAccountDoc.put(JSON_USER_ACCOUNT_BALANCE, satoshiIntToSatoshiString(userBalanceInSatoshis));
    sellerAccountDoc.erase(JSON_USER_ACCOUNT_LOCKED_CREDITTING_TRANSACTION);
    opDescription = "cas swap+credit+unlock the seller (" + transactionSeller + ") for transaction: " + keyToTransactionMaybeWithStateOfUncreditted;
    if(!couchbaseStoreRequestWithExponentialBackoff(userAccountKey(transactionSeller), Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(sellerAccountDoc), LCB_SET, sellerAccountLockedToCredittingTransactionCas, opDescription))
    {
        //db error
        return false;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        if(m_LastOpStatus == LCB_KEY_EEXISTS) //cas-swap fail
        {
            //back off to neighbor/self
            return false;
        }

        //error other than cas swap fail = db error, so spit out what we were doing and return false (which stops us)
        errorOutput("failed while: " + opDescription);
        return false;
    }

    //cas swap+credit+unlock the profile SUCCESS!

    return true;
}
void Abc2TransactionCreditor::creditTransactions()
{
    if(!connectToCouchbase())
    {
        emit transactionCredittingFinished(false);
        return;
    }
    if(m_Verbose)
        emit o("Connected...");

    lcb_set_http_complete_callback(m_Couchbase, Abc2TransactionCreditor::viewQueryCompleteCallbackStatic);

    std::string viewPath = "_design/AllTransactionsWithStateOfUncreditted/_view/AllTransactionsWithStateOfUncreditted?stale=ok"; //TODOmb: perhaps if this app is run on the webserver, we should use some form of pagination so that all the uncreditted transactions don't have to be in ram. for now KISS. Another way to solve it is to run this app on a server other than the webserver
    lcb_http_cmd_t viewQueryCmd;
    viewQueryCmd.version = 0;
    viewQueryCmd.v.v0.path = viewPath.c_str();
    viewQueryCmd.v.v0.npath = viewPath.length();
    viewQueryCmd.v.v0.body = NULL;
    viewQueryCmd.v.v0.nbody = 0;
    viewQueryCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    viewQueryCmd.v.v0.chunked = 0;
    viewQueryCmd.v.v0.content_type = "application/json";
    m_ViewQueryHadError = true; //prove to me that there wasn't an error ;-P
    lcb_error_t error = lcb_make_http_request(m_Couchbase, this, LCB_HTTP_TYPE_VIEW, &viewQueryCmd, NULL);
    if (error != LCB_SUCCESS)
    {
        emit e("Failed to query view " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        emit transactionCredittingFinished(false);
        return;
    }
    lcb_wait(m_Couchbase);
    if(m_ViewQueryHadError)
    {
        emit e("View query had error");
        emit transactionCredittingFinished(false);
        return;
    }

    //view successfully queried

    const ptree &rowsPt = m_ViewQueryPropertyTree.get_child("rows");
    m_UsersWithProfilesFoundLocked.clear();
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        const std::string &keyToTransactionMaybeWithStateOfUncreditted = row.second.get<std::string>("id");
        if(!creditTransactionIfStateReallyIsUncredittedAndReturnFalseIfWeShouldStopIteratingViewQueryResults(keyToTransactionMaybeWithStateOfUncreditted)) //TODOreq: unlike withdrawal request processor, this app will be run automatically (cron job or sleep'ing), so it needs to be more tolerant to errors
        {
            errorOutput("failed to credit transaction: " + keyToTransactionMaybeWithStateOfUncreditted);
            emit transactionCredittingFinished(false);
            return;
        }
    }

    if(m_TotalAmountCreditted > 0)
    {
        emit o("Amount creditted: " + QString::fromStdString(satoshiIntToJsonString(m_TotalAmountCreditted)));
        emit o("Amount earned: " + QString::fromStdString(satoshiIntToJsonString(m_TotalAmountEarned)));
    }
    emit transactionCredittingFinished(true);

}
