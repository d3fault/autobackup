#include "abc2withdrawrequestprocessor.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>

#include <QStringList>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

//TODOreq: I'm first going to code the "query view for all unprocessed" typical non-error case, BUT in order of program execution, the checking for previously failed "processing" (and possibly 'processedStillNeedToDeduct') states needs to come before that. I should definitely spit out that there are previosuly failed withdraw requests, and NOT continue onto querying view for all unprocessed states (until those fails are handled or ignored or whatever)
//TODOreq: for now I'm just going to "process" the requests, but unless I have some kind of "this is how much I'm about to deduct: XXX. Does this make sense?" (and then I manually make sure that that much in bitcoins have been received)... followed by "press Y or N to continue or not"... then this app may as well be completely automated...
//TODOreq: abc2 and this app both need to account for the new locking mode, AND SO DOES the pessimistic recovery app. I have no idea what the implications of that are. OT: I'm scared as fuck to modify this locking code, and yet it's pretty much the only thing standing in the way of completion (aside from some polish etc)
//I'm leaning towards having this app ONLY do the db stuff (deducting balances, settings withdrawal requests to processed -- BUT BE CAREFUL TO BE ABLE TO ROLL BACK IF SHIT DOESN'T ADD UP RIGHT) with respect to withdrawals, and to have it spit out some xml/json/whatever that is then taken to a "OpenBSD payout box" (used for nothing else), so I can then analyze that list to make sure the total payout amounts look right. but maybe that functionality should be in this app, I am unsure
Abc2WithdrawRequestProcessor::Abc2WithdrawRequestProcessor(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
{ }
void Abc2WithdrawRequestProcessor::errorOutput(const string &errorString)
{
    QString errorQString = QString::fromStdString(errorString);
    emit e(errorQString);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    (const_cast<Abc2WithdrawRequestProcessor*>(static_cast<const Abc2WithdrawRequestProcessor*>(cookie)))->viewQueryCompleteCallback(error, resp);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp)
{
    emit o("viewQueryCompleteCallback");
    if(error != LCB_SUCCESS)
    {
        emit e("httpCompleteCallback has error");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //TODOreq: don't trust the view query. we still must get the doc (and we have to anyways to get the CAS in order to fuck with it. so that being said, i now know i can emit(null,null) (which gives us only the key) in the view that's a WIP-in-mah-brain) and check AGAIN then that the state is 'unprocessed'. hence the name of the variable:
    //QList<QString> listOfKeysWhereViewQuerySaidTheStateIsUnprocessedButHeyMaybeThatWasStaleInfo;

    QByteArray jsonResultByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonResultString(jsonResultByteArray);

    ptree pt;
    std::string jsonResultStdString = jsonResultString.toStdString();
    std::istringstream is(jsonResultStdString);
    read_json(is, pt);
    boost::optional<std::string> errorMaybe = pt.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        emit e("json had error set");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    const ptree &rowsPt = pt.get_child("rows");
    m_UsersWithLockedProfiles.clear();
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        QString currentKeyToMaybeUnprocessedWithdrawalRequest = QString::fromStdString(row.second.get<std::string>("id"));
        if(!processWithdrawalRequestAndReturnFalseOnError(currentKeyToMaybeUnprocessedWithdrawalRequest))
        {
            emit e("Stopping becaue there was an error while processing withdraw request with key: " + currentKeyToMaybeUnprocessedWithdrawalRequest);
            emit withdrawalRequestProcessingFinished(false);
            return;
        }
        //listOfKeysWhereViewQuerySaidTheStateIsUnprocessedButHeyMaybeThatWasStaleInfo.append(QString::fromStdString(row.second.get<std::string>("id")));
    }
    /*Q_FOREACH(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest, listOfKeysWhereViewQuerySaidTheStateIsUnprocessedButHeyMaybeThatWasStaleInfo)
    {
    }*/

    emit o("Withdrawal Request Processing Finished Successfully");
    emit withdrawalRequestProcessingFinished(true);
}
bool Abc2WithdrawRequestProcessor::processWithdrawalRequestAndReturnFalseOnError(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest)
{
    //"maybe" unprocessed because we don't trust the view query

    //check state is really unprocessed
    std::string currentKeyToMaybeUnprocessedWithdrawalRequestStdString = currentKeyToMaybeUnprocessedWithdrawalRequest.toStdString();
    std::string getDescription = "the withdrawal request in state unprocessed for the first step of processing: " + currentKeyToMaybeUnprocessedWithdrawalRequestStdString;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(currentKeyToMaybeUnprocessedWithdrawalRequestStdString, getDescription))
    {
        return false;
    }
    lcb_cas_t withdrawalRequestCas = m_LastGetCas;
    ptree withdrawalRequestPt;
    std::istringstream withdrawalRequestIs(m_LastDocGetted);
    read_json(withdrawalRequestIs, withdrawalRequestPt);
    std::string withdrawFundsRequestState = withdrawalRequestPt.get<std::string>(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY);
    if(withdrawFundsRequestState != JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED)
    {
        //the view query was stale (despite stale=false) and the state was not in fact unprocessed... so we just return true and continue onto the next one ('processing' and other error states are handled at a different point (before app even does it's normal thing))
        return true;
    }

    //state is 'unprocessed'

    //check profile not locked attempting to fill slot (or locked withdrawing)-- TODOreq: "slot attempting to fill" and "add funds" code needs to check that profile isn't locked in this new "withdraw funds" lock mode
    QStringList keyParts = currentKeyToMaybeUnprocessedWithdrawalRequest.split(D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR);
    const QString &userRequestingWithdrawal = keyParts.at(1);
    if(m_UsersWithLockedProfiles.contains(userRequestingWithdrawal))
    {
        return true; //we already checked them and they have a locked profile, so we don't check any of their other withdrawal requests
    }
    std::string userRequestingWithdrawalStdString = userRequestingWithdrawal.toStdString();
    getDescription = "the profile request to see if it's locked and then to lock it for user: " + userRequestingWithdrawalStdString;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(userRequestingWithdrawalStdString), getDescription))
    {
        //error getting profile doc
        return false;
    }
    lcb_cas_t userProfileCas = m_LastGetCas;
    ptree userProfilePt;
    std::istringstream userProfileIs(m_LastDocGetted);
    read_json(userProfileIs, userProfilePt);

    if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(userProfilePt))
    {
        m_UsersWithLockedProfiles.append(userRequestingWithdrawal);
        emit o("user profile '" + userRequestingWithdrawal + "' is locked, continuing onto next. no more of this user's withdrawal requests will be processed and you must run this app again after fixing the locked account problem manually"); //TO DOne req: append to list of locked profiles, so we don't check again later for all their other withdrawal requests (especially since that would break ordering for processing of withdrawal requests. now changing from optimization to req)
        return true; //just continue onto next for now. the above output is kind of like a "warning". it's not fatal, but we do need to intervene...
    }

    //TODOreq: verify sufficient funds and don't continue if not enough
    const std::string &userBalanceInSatoshisStr = userProfilePt.get<std::string>(JSON_USER_ACCOUNT_BALANCE);
    SatoshiInt userBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(userBalanceInSatoshisStr);

    std::string amountToWithdrawJsonString = withdrawalRequestPt.get<std::string>(JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT);
    SatoshiInt amountToWithdrawInSatoshis = boost::lexical_cast<SatoshiInt>(amountToWithdrawJsonString);

    if(userBalanceInSatoshis < amountToWithdrawInSatoshis)
    {
        //insufficient funds. not fatal error, they might have simply spent the funds on an ad slot filler after filing the withdrawal request. set the withdrawal request state to insiffucient funds done and return true;
        withdrawalRequestPt.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_INSUFFICIENTFUNDS);
        std::string insufficientFundsWithdrawalRequest = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(withdrawalRequestPt);
        if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(currentKeyToMaybeUnprocessedWithdrawalRequestStdString, insufficientFundsWithdrawalRequest, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + currentKeyToMaybeUnprocessedWithdrawalRequestStdString + "' to insufficient funds"))
        {
            return false; //on the other hand, failing to set the withdrawal funds request to insufficient funds IS fatal error
        }
        return true;
    }

    //profile not locked and there are sufficient funds. lock profile.
    userProfilePt.put(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, currentKeyToMaybeUnprocessedWithdrawalRequestStdString);
    std::ostringstream userProfileLockedForWithdrawingOs;
    write_json(userProfileLockedForWithdrawingOs, userProfilePt, false);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(userAccountKey(userRequestingWithdrawalStdString), userProfileLockedForWithdrawingOs.str(), LCB_SET, userProfileCas, "locking user profile '" + userRequestingWithdrawalStdString + "' for withdrawal of funds request '" +  currentKeyToMaybeUnprocessedWithdrawalRequestStdString + "'"))
    {
        //error locking profile doc. TODOreq: it may be a non-fatal error in that the cas swap simply failed (they are attempting to purchase a slot and the profile was locked? (or even they are adding funds or shit changing their password. tons of reasons this could fail) nothing wrong with that). right now we have 'return false', which indicates a fatal error. m_LastOpStatus is your friend here, and maybe i shouldn't use the "requiring success" method, idfk
        return false;
    }

    //cas swap set withdrawal request state to 'processing'
    withdrawalRequestPt.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSING);
    std::string withdrawalRequestInStateProcessingJson = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(withdrawalRequestPt);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(currentKeyToMaybeUnprocessedWithdrawalRequestStdString, withdrawalRequestInStateProcessingJson, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + currentKeyToMaybeUnprocessedWithdrawalRequestStdString + "' to processing"))
    {
        //error setting withdrawal request state to processing
        return false;
    }

    //withdrawal request state is 'processing', so PROCESS! i need to decide if i'm list building or calling bitcoind... hmmm.....


    return true;
}
void Abc2WithdrawRequestProcessor::processWithdrawalRequests()
{
    if(!connectToCouchbase())
        return;
    emit o("Connected...");

    lcb_set_http_complete_callback(m_Couchbase, Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic);

    std::string viewPath = "_design/dev_AllWithdrawalRequestsWithStateOfUnprocessed/_view/AllWithdrawalRequestsWithStateOfUnprocessed?stale=false"; //TODOmb: perhaps if this app is run on the webserver, we should use some form of pagination so that all the requests don't have to be in ram. for now KISS. Another way to solve it is to run this app on a server other than the webserver
    lcb_http_cmd_t queryPageOfViewCmd;
    queryPageOfViewCmd.version = 0;
    queryPageOfViewCmd.v.v0.path = viewPath.c_str();
    queryPageOfViewCmd.v.v0.npath = viewPath.length();
    queryPageOfViewCmd.v.v0.body = NULL;
    queryPageOfViewCmd.v.v0.nbody = 0;
    queryPageOfViewCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    queryPageOfViewCmd.v.v0.chunked = 0;
    queryPageOfViewCmd.v.v0.content_type = "application/json";
    lcb_error_t error = lcb_make_http_request(m_Couchbase, this, LCB_HTTP_TYPE_VIEW, &queryPageOfViewCmd, NULL);
    if (error != LCB_SUCCESS)
    {
        emit e("Failed to query view " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_wait(m_Couchbase);
}
