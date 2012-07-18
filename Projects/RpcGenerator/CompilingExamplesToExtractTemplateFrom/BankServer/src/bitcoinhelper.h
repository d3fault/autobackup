#ifndef BITCOINHELPER_H
#define BITCOINHELPER_H

#include <QObject>

#include "../../RpcBankServerAndClientShared/broadcastdispensers.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h"

class BitcoinHelper : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinHelper();
    void takeOwnershipOfApplicableBroadcastDispensers(BroadcastDispensers *broadcastDispensers);
private:
    PendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;
};

#endif // BITCOINHELPER_H
