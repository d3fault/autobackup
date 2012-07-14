#ifndef IRPCBANKSERVERCLIENTSHELPER_H
#define IRPCBANKSERVERCLIENTSHELPER_H

#include <QObject>
#include <QThread>

#include "messageDispensers/actiondispensers.h"
#include "messageDispensers/broadcastdispensers.h"

class IRpcBankServer;
class IRpcBankServerMessageTransporter;

class RpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerClientsHelper(IRpcBankServer *business);

    //thread-safe helper methods
    void init();
    void start();
    void stop();

    //broadcast dispenser elections
    PendingBalanceAddedMessageDispenser *takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(QObject *owner);
    //etc

    //actions dispenser elections -- on rpc server, we take ownership etc. but good to use a consistent method of taking ownership of dispensers. i may even be able to re-use the code in the rpc client... where the business impl (rpc client business impl) does take ownership of every action dispenser
    CreateBankAccountMessageDispenser *takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner);
private:
    IRpcBankServer *m_Business;
    IRpcBankServerMessageTransporter *m_Transporter; //ITransport is "ProtocolKnower", the impl hasA SslTcpServer* and uses it directly~
    //^^we're still going to use an interface approach, but only our cpp file will know of "ProtocolKnower", so it can 'new' it

    QThread *m_BusinessThread;
    QThread *m_RpcBankServerMessageTransporterThread;

    ActionDispensers *m_ActionDispensers;
    BroadcastDispensers *m_BroadcastDispensers;

    void connectTransporterActionRequestSignalsToBusinessSlots();   
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    void simulateCreateBankAccount();
};

#endif // IRPCBANKSERVERCLIENTSHELPER_H
