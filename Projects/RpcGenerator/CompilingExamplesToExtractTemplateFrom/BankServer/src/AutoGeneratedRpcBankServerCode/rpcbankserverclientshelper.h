#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>
#include <QThread>
#include <QHostAddress>

#include "MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"
#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h"
#include "ibankserverprotocolknower.h"
#include "../../GlobalRpcShared/multiserverabstraction.h"

class RpcBankServerClientsHelper : public IBankServerProtocolKnower
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(QObject *parent = 0);
    inline RpcBankServerBroadcastDispensers *broadcastDispensers() { return m_BroadcastDispensers; }
private:
    inline void emitInitializedSignalIfReady() { if(checkInitializedAndAllBroadcastDispensersClaimed()) emit initialized(); }
    inline bool checkInitializedAndAllBroadcastDispensersClaimed() { return (m_Initialized && m_BroadcastDispensers->everyDispenserIsCreated()); }
    bool m_Initialized;

    RpcBankServerActionDispensers *m_ActionDispensers;
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;

    MultiServerAbstraction m_MultiServerAbstraction;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void initialize(MultiServerAbstractionArgs multiServerAbstractionArgs);
    void start();
    void stop();

    void handleDoneClaimingBroadcastDispensers();
};

#endif // RPCBANKSERVERCLIENTSHELPER_H
