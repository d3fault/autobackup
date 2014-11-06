#ifndef PAGINATIONOFVIEWQUERY_H
#define PAGINATIONOFVIEWQUERY_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

#include <QMap>
#include <QStack>

#include "paginationofviewquerycommon.h"

#define CACHED_VIEW_QUERY_PAGES_HASH_VALUE_TYPE QPair<std::string /* last docid */, ViewQueryPageContentsType /* usernames on that page */> /*this comment is only here to keep Qt creator from deleting trailing whitespace at save. delete it and watch what happens i double dog dare you ;-P */
#define CACHED_VIEW_QUERY_PAGES_HASH_KEY_AND_VALUE_TYPE int /*pageNum*/, CACHED_VIEW_QUERY_PAGES_HASH_VALUE_TYPE

class PaginationOfViewQuery : public QObject, public ISynchronousLibCouchbaseUser /*should be couchbase client, not couchbase user. w/e*/
{
    Q_OBJECT
public:
    explicit PaginationOfViewQuery(QObject *parent = 0);
protected:
    void errorOutput(const std::string &errorString);
private:
    QMap<CACHED_VIEW_QUERY_PAGES_HASH_KEY_AND_VALUE_TYPE> m_CachedPagesAndTheirLastDocIdsAndLastKeys;

    static void httpCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void httpCompleteCallback(int pageNum_WithOneBeingTheFirstPage, lcb_error_t error, const lcb_http_resp_t *resp);
signals:
    void paginationOfViewQueryInitialized(bool success);
    void finishedQueryingPageOfView(const ViewQueryPageContentsType &pageContents);
    void quitRequested();
public slots:
    void initializePaginationOfViewQuery();
    void queryPageOfView(int pageNum_WithOneBeingTheFirstPage);
};

#endif // PAGINATIONOFVIEWQUERY_H
