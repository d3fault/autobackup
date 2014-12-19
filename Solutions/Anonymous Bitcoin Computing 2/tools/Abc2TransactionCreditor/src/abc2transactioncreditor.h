#ifndef ABC2TRANSACTIONCREDITOR_H
#define ABC2TRANSACTIONCREDITOR_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

#include <QSet>

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
    QSet<QString> m_UsersWithProfilesFoundLocked; //TODOoptimization: withdrawal request processor shouldn't use QStringList either

    void errorOutput(const string &errorString);

    static void viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp);

    bool creditTransactionIfStateReallyIsUncredittedAndReturnFalseIfWeShouldStopIteratingViewQueryResults(const std::string &keyToTransactionMaybeWithStateOfUncreditted);
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void transactionCredittingFinished(bool success);
public slots:
    void creditTransactions();
};

#endif // ABC2TRANSACTIONCREDITOR_H
