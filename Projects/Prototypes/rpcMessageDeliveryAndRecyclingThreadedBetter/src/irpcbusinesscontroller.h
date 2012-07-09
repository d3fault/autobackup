#ifndef IRPCCLIENTSHELPER_H
#define IRPCCLIENTSHELPER_H

#include <QObject>
#include <QThread>

class IRpcBusiness;

class IRpcBusinessController : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBusinessController(IRpcBusiness *business);

    //thread-safe helper methods
    void init();
    void start();
    void stop();

    //message dispenser elections
    void setBroadcastDispenserParentForPendingBalanceAddedMessages(QObject *owner);
    //etc
private:
    IRpcBusiness *m_Business;
    IRpcTransport *m_Transport; //ITransport is "ProtocolKnower", the impl hasA SslTcpServer* and uses it~

    QThread *m_BusinessThread;
    QThread *m_RpcTransportThread;
protected:
    void setupConnections();
signals:
    void initialized();
    void started();
    void stopped();
};

#endif // IRPCCLIENTSHELPER_H
