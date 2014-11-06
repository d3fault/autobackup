#include "paginationofviewquery.h"

#include <QDebug>

#include <libcouchbase/http.h>

PaginationOfViewQuery::PaginationOfViewQuery(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
{ }
void PaginationOfViewQuery::errorOutput(const string &errorString)
{
    qDebug() << QString::fromStdString(errorString);
    emit quitRequested();
}
void PaginationOfViewQuery::httpCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    const_cast<PaginationOfViewQuery*>(static_cast<const PaginationOfViewQuery*>(cookie))->httpCompleteCallback(request, instance, error, resp);
}
void PaginationOfViewQuery::httpCompleteCallback(lcb_http_request_t request, lcb_t instance, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    qDebug() << "httpCompleteCallback";
    if(error != LCB_SUCCESS)
    {
        qDebug() << "httpCompleteCallback has error";
        emit quitRequested();
        return;
    }
    QByteArray path(resp->v.v0.path, resp->v.v0.npath);
    qDebug() << path;
    QByteArray jsonPageContentsByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonPageContentsString(jsonPageContentsByteArray);
    emit finishedQueryingPageOfView(jsonPageContentsString);
}
void PaginationOfViewQuery::initializePaginationOfViewQuery()
{
    if(!connectToCouchbase())
    {
        emit paginationOfViewQueryInitialized(false);
        return;
    }
    lcb_set_http_complete_callback(m_Couchbase, PaginationOfViewQuery::httpCompleteCallbackStatic);
    emit paginationOfViewQueryInitialized(true);

    //kind of a hack, so that when the gui loads for the first time we get the first page right away
    queryPageOfView(0);
}
void PaginationOfViewQuery::queryPageOfView(int pageNum)
{
    lcb_http_cmd_t queryPageOfViewCmd;
    queryPageOfViewCmd.version = 0;
    std::string viewPath = "_design/dev_AllUsersWithAtLeastOneAdCampaign/_view/AllUsersWithAtLeastOneAdCampaign?stale=false&limit=1&skip="; //once I upgrade to couchbase 3.0, I'll use stale=ok :-D. //TODOreq: since the keys of a view result are already sorted, my reduce function can be made more efficient by simply seeing if the next "key" was the same as the previous "key", instead of iterating the ENTIRE list of "ret" to see if the key is already in it
    viewPath += QString::number(pageNum).toStdString();
    queryPageOfViewCmd.v.v0.path = viewPath.c_str();
    queryPageOfViewCmd.v.v0.npath = strlen(queryPageOfViewCmd.v.v0.path);
    queryPageOfViewCmd.v.v0.body = NULL;
    queryPageOfViewCmd.v.v0.nbody = 0;
    queryPageOfViewCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    queryPageOfViewCmd.v.v0.chunked = 0;
    queryPageOfViewCmd.v.v0.content_type = "application/json";
    lcb_error_t error = lcb_make_http_request(m_Couchbase, this, LCB_HTTP_TYPE_VIEW, &queryPageOfViewCmd, NULL);
    if (error != LCB_SUCCESS)
    {
        qDebug() <<  "Failed to query view" << QString::fromStdString(lcb_strerror(m_Couchbase, error));
        emit quitRequested();
        return;
    }
    lcb_wait(m_Couchbase);
}
