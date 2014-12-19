#include "abc2transactioncreditor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <libcouchbase/http.h>

#include "abc2couchbaseandjsonkeydefines.h"

//TODOreq: abc2 needs a message somewhere telling campaign owners about the delayed creditting mechanism
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
bool Abc2TransactionCreditor::creditTransactionIfStateReallyIsUncredittedAndReturnFalseOnError(const string &keyToTransactionMaybeWithStateOfUncreditted)
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
        //the view query was stale (despite stale=false) and the state was not in fact uncreditted... so we just return true and continue onto the next one ('creditting' and other error states are handled at a different point (before app even does it's normal thing))
        return true;
    }

    //state is 'uncreditted'

    const std::string &transactionSeller = transactionDoc.get<std::string>(JSON_TRANSACTION_SELLER);
    TODO LEFT OFF

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
        if(!creditTransactionIfStateReallyIsUncredittedAndReturnFalseOnError(keyToTransactionMaybeWithStateOfUncreditted)) //TODOreq: unlike withdrawal request processor, this app will be run automatically (cron job or sleep'ing), so it needs to be more tolerant to errors
        {
            errorOutput("failed to credit transaction: " + keyToTransactionMaybeWithStateOfUncreditted);
            emit transactionCredittingFinished(false);
            return;
        }
    }

    //TODOreq: somwhere later: emit transactionCredittingFinished(true);
}
