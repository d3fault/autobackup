#ifndef ABC2WITHDRAWREQUESTPROCESSOR_H
#define ABC2WITHDRAWREQUESTPROCESSOR_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

class Abc2WithdrawRequestProcessor : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    explicit Abc2WithdrawRequestProcessor(QObject *parent = 0);
private:
    void errorOutput(const string &errorString);

    static void viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp);

    bool processWithdrawalRequestAndReturnWhetherOrNotItSucceeded(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest);
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void withdrawalRequestProcessingFinished(bool success);
public slots:
    void processWithdrawalRequests();
};

#endif // ABC2WITHDRAWREQUESTPROCESSOR_H
