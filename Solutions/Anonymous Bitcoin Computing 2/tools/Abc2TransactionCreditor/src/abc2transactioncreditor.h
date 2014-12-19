#ifndef ABC2TRANSACTIONCREDITOR_H
#define ABC2TRANSACTIONCREDITOR_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

#include <QStringList>

#include <boost/property_tree/ptree.hpp>

using namespace boost::property_tree;

class Abc2TransactionCreditor : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    explicit Abc2TransactionCreditor(QObject *parent = 0);
private:
    bool m_ViewQueryHadError;
    ptree m_ViewQueryPropertyTree;
    QStringList m_UsersWithProfilesFoundLocked;

    void errorOutput(const string &errorString);

    static void viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp);

    bool creditTransactionIfStateReallyIsUncredittedAndReturnFalseOnError(const std::string &keyToTransactionMaybeWithStateOfUncreditted);
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void transactionCredittingFinished(bool success);
public slots:
    void creditTransactions();
};

#endif // ABC2TRANSACTIONCREDITOR_H
