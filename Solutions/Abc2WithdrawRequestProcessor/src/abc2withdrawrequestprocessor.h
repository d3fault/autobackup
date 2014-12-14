#ifndef ABC2WITHDRAWREQUESTPROCESSOR_H
#define ABC2WITHDRAWREQUESTPROCESSOR_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

#include <boost/property_tree/ptree.hpp>

#include <QStringList>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

class QNetworkAccessManager;
class QNetworkReply;

class Abc2WithdrawRequestProcessor : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    explicit Abc2WithdrawRequestProcessor(QObject *parent = 0);
private:
    bool m_ViewQueryHadError;
    ptree m_ViewQueryPropertyTree;
    QStringList m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate;
    int m_CurrentIndexIntoViewQueryResultsStringList;
    int m_ViewQueryResultsStringListSize;
    QStringList m_UsersWithProfilesFoundLocked;
    std::string m_CurrentKeyToMaybeUnprocessedWithdrawalRequest;
    std::string m_CurrentUserRequestingWithdrawal;
    ptree m_CurrentWithdrawalRequestPropertyTree;
    ptree m_CurrentUserProfilePropertyTree;
    SatoshiInt m_CurrentUserBalanceInSatoshis;
    SatoshiInt m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis;
    SatoshiInt m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFee;
    lcb_cas_t m_CurrentUserProfileInLockedWithdrawingStateCas;
    lcb_cas_t m_CurrentWithdrawalRequestInProcessingStateCas;
    QString m_CurrentBitcoinCommand;
    QNetworkAccessManager *m_NetworkAccessManager;

    void errorOutput(const string &errorString);

    static void viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp);
    void viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp);

    void processNextWithdrawalRequestOrEmitFinishedIfNoMore();

    void processWithdrawalRequest(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest);
signals:
    void e(const QString &msg);
    void o(const QString &msg);
    void withdrawalRequestProcessingFinished(bool success);
public slots:
    void processWithdrawalRequests();
private slots:
    void handleNetworkReplyFinished(QNetworkReply *reply);
};

#endif // ABC2WITHDRAWREQUESTPROCESSOR_H
