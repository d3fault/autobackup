#ifndef RPCBANKSERVERHELPER_H
#define RPCBANKSERVERHELPER_H

#include <QObject>
#include <QThread>

#include "irpcbankserverclientbusiness.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h"

class RpcBankServerHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerHelper(IRpcBankServerClientBusiness *rpcBankServerClient);
    void init();
    void start();
    void stop();
private:
    QThread *m_BusinessThread;
    IRpcBankServerClientBusiness *m_RpcBankServerClient;
    RpcBankServerActionDispensers *m_ActionDispensers;
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;
    QThread *m_TransporterThread;
    IBankServerClientProtocolKnower *m_Transporter;

    void moveTransporterToItsOwnThreadAndStartTheThread();
    void moveBusinessToItsOwnThreadAndStartTheThread();

    void daisyChainInitStartStopConnections();
signals:
    void initialized();
    void started();
    void stopped();
};

#endif // RPCBANKSERVERHELPER_H
