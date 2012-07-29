#ifndef SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
#define SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H

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
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *newClient);
    void handleMessageReceivedFromRpcClientOverNetwork();
public slots:
    void init();
    void start();
    void stop();
};

#endif // SSLTCPSERVERANDBANKSERVERPROTOCOLKNOWER_H
