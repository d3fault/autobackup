#include "abc2transactioncreditor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>

#include "abc2couchbaseandjsonkeydefines.h"


//The flow goes like this: query a view to get all transactions with state=uncreditted, iterate them. Here's one transaction flow: verify state is uncreditted (view may have been stale. a state of 'creditting' means we are done (neighbor on it), a state of 'creditted' just means skip that transaction). cas swap lock the transaction seller's 'profile' requiring success (fail means they are doing other stuff. not fatal but we don't try to credit any more of that user's transactions during this 'run')  into "credittingTransaction: txKey" mode. cas swap requiring success the transaction from state 'uncreditted' to 'creditting' (cas swap fail means 'neighbor'/us is doing it so we back off ON ALL THE REST OF THE VIEW QUERY TODOreq. we are basically done completely). cas swap+credit+unlock the profile for the full amount specified in the transaction (fail means a neighbor/'us' is on it and so we are done with rest of view query). cas swap the transaction from state creditting to creditted (fail means neighbor is on it (semi-recovery (false positive, the recovery STOLE the execution lewl -- fugtit (<- cool typo))) [...])

//TODOreq: abc2 needs a message somewhere telling campaign owners about the delayed creditting mechanism. the campaign creation page seems like a good spot
//TODOreq: finding a transaction in state "creditting" where the seller doesn't have the profile locked means we failed after unlocking+creditting the profile. As error recovery, we simply change the state from creditting to creditted (ofc if the profile is locked, we know where to resume as well)
Abc2TransactionCreditor::Abc2TransactionCreditor(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
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
    const std::string &transactionState = transactionDoc.get<std::string>(JSON_TRANSACTION_STATE_KEY); //TODOreq: abc2 needs to set this state when creating the doc. so does pessimistic recoverer. there might be multiple places in abc2 where a tx doc is created (inline/login recovery)
    if(transactionState != JSON_TRANSACTION_STATE_VALUE_UNCREDITTED)
    {
        //the view query was stale (despite stale=false) and the state was not in fact uncreditted... so we just return true and continue onto the next one ('creditting' and other error states are handled at a different point (probably (tbd) before app even does it's normal thing))
        return true;
    }

    //state is 'uncreditted'

    //get seller profile to see if locked (if unlocked, lock)
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
            m_UsersWithProfilesFoundLocked.insert(transactionSellerQString);
            return true;
        }

        //error other than cas swap fail = db error, so return false (which stops us)
        errorOutput("failed while: " + opDescription);
        return false;
    }
    lcb_cas_t sellerAccountLockedToCredittingTransactionCas = m_LastSetCas;

    //profile locked to 'creditting transaction'

    //change transaction state from 'uncreddited' to 'creditting'
    transactionDoc.put(JSON_TRANSACTION_STATE_KEY, JSON_TRANSACTION_STATE_VALUE_CREDITTING);
    opDescription = "changing transaction state from uncreditted to creditting: " + keyToTransactionMaybeWithStateOfUncreditted;
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
    lcb_cas_t transactionDocInStateCredittingCas = m_LastSetCas;

    //transaction doc in state 'creditting'

    //cas swap+credit+unlock the profile
    SatoshiInt transactionAmountInSatoshis = boost::lexical_cast<SatoshiInt>(transactionDoc.get<std::string>(JSON_TRANSACTION_AMOUNT));
    SatoshiInt userBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(sellerAccountDoc.get<std::string>(JSON_USER_ACCOUNT_BALANCE));
    userBalanceInSatoshis += transactionAmountInSatoshis;
    sellerAccountDoc.put(JSON_USER_ACCOUNT_BALANCE, satoshiIntToSatoshiString(userBalanceInSatoshis));
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

    //change transaction state from creditting to creditted
    transactionDoc.put(JSON_TRANSACTION_STATE_KEY, JSON_TRANSACTION_STATE_VALUE_CREDITTED);
    opDescription = "changing transaction state from creditting to creditted: " + keyToTransactionMaybeWithStateOfUncreditted;
    if(couchbaseStoreRequestWithExponentialBackoff(keyToTransactionMaybeWithStateOfUncreditted, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(transactionDoc), LCB_SET, transactionDocInStateCredittingCas, opDescription))
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

    return true;
}
void Abc2TransactionCreditor::creditTransactions()
{
    if(!connectToCouchbase())
    {
        emit transactionCredittingFinished(false);
        return;
    }
    emit o("Connected...");

    lcb_set_http_complete_callback(m_Couchbase, Abc2TransactionCreditor::viewQueryCompleteCallbackStatic);

    std::string viewPath = "_design/dev_AllTransactionsWithStateOfUncreditted/_view/AllTransactionsWithStateOfUncreditted?stale=false"; //TODOmb: perhaps if this app is run on the webserver, we should use some form of pagination so that all the uncreditted transactions don't have to be in ram. for now KISS. Another way to solve it is to run this app on a server other than the webserver
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

    emit transactionCredittingFinished(true);
}
