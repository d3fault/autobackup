#ifndef RPCBANKSERVERCLIENTSHELPER_H
#define RPCBANKSERVERCLIENTSHELPER_H

#include <QObject>
#include <QThread>

#include "irpcbankserverbusiness.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"
#include "MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h"
#include "MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h"
#include "ibankserverprotocolknower.h"

class RpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(IRpcBankServerBusiness *rpcBankServer);
    void init();
    void start();
    void stop();
private:
    QThread *m_BusinessThread;
    IRpcBankServerBusiness *m_RpcBankServer;
    RpcBankServerActionDispensers *m_ActionDispensers;
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;
    QThread *m_TransporterThread;
    IBankServerProtocolKnower *m_Transporter;

    void moveTransporterToItsOwnThreadAndStartTheThread();
    void moveBusinessToItsOwnThreadAndStartTheThread();

    //void actualRpcConnections();
    void daisyChainInitStartStopConnections();
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
};

#endif // RPCBANKSERVERCLIENTSHELPER_H
