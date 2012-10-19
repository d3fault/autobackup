#ifndef SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
#define SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H

#include <QHash>

#include "ibankserverprotocolknower.h"

#include "network/ssl/ssltcpserver.h"

class SslTcpServerAndBankServerProtocolKnower : public IBankServerProtocolKnower
{
    Q_OBJECT
public:
    explicit SslTcpServerAndBankServerProtocolKnower(QObject *parent = 0);
protected:
    void myTransmit(IMessage *message, uint uniqueRpcClientId);
    void myBroadcast(IMessage *message);
private:
    SslTcpServer *m_SslTcpServer;
    //QHash<QIODevice*,RpcBankServerMessageHeader*> m_HashOfHeadersBySocketAwaitingMoreData;
    QHash<QIODevice*,IRecycleableAndStreamable*> m_HashOfMessagesBySocketAwaitingMoreData; //TODOoptimization: is there another way to do this without having to look into a hash every time? Actually it doesn't sound _THAT_ expensive, because the size of the hash will only be the size of the list of our connected clients, maximum (usually less)
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *newClient);
    void handleMessageReceivedFromRpcClientOverNetwork();
public slots:
    void init();
    void start();
    void stop();
};

#endif // SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
