#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>

#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h"

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper();
    void takeOwnershipOfApplicableBroadcastDispensers(RpcBankServerBroadcastDispensers *broadcastDispensers);
private:
    PendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void init();
    void start();
    void stop();
};

#endif // BITCOINHELPER_H
