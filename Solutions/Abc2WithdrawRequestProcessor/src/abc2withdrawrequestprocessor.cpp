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
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        QString currentKeyToMaybeUnprocessedWithdrawalRequest = QString::fromStdString(row.second.get<std::string>("id"));
        if(!processWithdrawalRequestAndReturnWhetherOrNotItSucceeded(currentKeyToMaybeUnprocessedWithdrawalRequest))
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
bool Abc2WithdrawRequestProcessor::processWithdrawalRequestAndReturnWhetherOrNotItSucceeded(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest)
{
    //"maybe" unprocessed because we don't trust the view query

    //check state is really unprocessed
    std::string currentKeyToMaybeUnprocessedWithdrawalRequestStdString = currentKeyToMaybeUnprocessedWithdrawalRequest.toStdString();
    std::string getDescription = "the withdrawal request in state unprocessed for the first step of processing: " + currentKeyToMaybeUnprocessedWithdrawalRequestStdString;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(currentKeyToMaybeUnprocessedWithdrawalRequestStdString, getDescription))
    {
        return false;
    }
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
    std::string userRequestingWithdrawalStdString = userRequestingWithdrawal.toStdString();
    getDescription = "the profile request to see if it's locked and then to lock it for user: " + userRequestingWithdrawalStdString;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(userRequestingWithdrawalStdString), getDescription))
    {
        //error getting profile doc
        return false;
    }
    ptree userProfilePt;
    std::istringstream userProfileIs(m_LastDocGetted);
    read_json(userProfileIs, userProfilePt);

    TODO LEFT OFF CHECK LOCKED BLAH

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
