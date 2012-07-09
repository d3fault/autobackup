#ifndef IRPCBANKSERVERCLIENTSHELPER_H
#define IRPCBANKSERVERCLIENTSHELPER_H

#include <QObject>
#include <QThread>

#include "messageDispensers/broadcastdispensers.h"
#include "irpcbankservermessagetransporter.h"

class IRpcBankServer;
class PendingBalanceAddedMessageDispenser;

class IRpcBankServerClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBankServerClientsHelper(IRpcBankServer *business);

    //thread-safe helper methods
    void init();
    void start();
    void stop();

    //message dispenser elections
    PendingBalanceAddedMessageDispenser *takeOwnershipOfPendingBalanceAddedMessageDispenserRiggedForDelivery(QObject *owner);
    //etc
private:
    IRpcBankServer *m_Business;

    QThread *m_BusinessThread;
    QThread *m_RpcBankServerMessageTransporterThread;

    BroadcastDispensers *m_BroadcastDispensers;

    void connectTransporterActionRequestSignalsToBusinessSlots();
protected:
    IRpcBankServerMessageTransporter *m_Transporter; //ITransport is "ProtocolKnower", the impl hasA SslTcpServer* and uses it directly~
    virtual IRpcBankServerMessageTransporter *getNewTransporterImpl()=0;
signals:
    void initialized();
    void started();
    void stopped();
};

#endif // IRPCBANKSERVERCLIENTSHELPER_H
