#ifndef PAGINATIONOFVIEWQUERY_H
#define PAGINATIONOFVIEWQUERY_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

class PaginationOfViewQuery : public QObject, public ISynchronousLibCouchbaseUser /*should be couchbase client, not couchbase user. w/e*/
{
    Q_OBJECT
public:
    explicit PaginationOfViewQuery(QObject *parent = 0);
protected:
    void errorOutput(const std::string &errorString);
private:
    static void httpCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void httpCompleteCallback(lcb_http_request_t request, lcb_t instance, lcb_error_t error, const lcb_http_resp_t *resp);
signals:
    void paginationOfViewQueryInitialized(bool success);
    void finishedQueryingPageOfView(const QString &jsonPageContents);
    void quitRequested();
public slots:
    void initializePaginationOfViewQuery();
    void queryPageOfView(int pageNum);
};

#endif // PAGINATIONOFVIEWQUERY_H
