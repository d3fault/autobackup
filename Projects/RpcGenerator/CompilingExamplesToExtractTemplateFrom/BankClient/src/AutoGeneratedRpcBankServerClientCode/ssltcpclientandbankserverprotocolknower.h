#ifndef SSLTCPCLIENTANDBANKSERVERPROTOCOLKNOWER_H
#define SSLTCPCLIENTANDBANKSERVERPROTOCOLKNOWER_H

#if 0
#include "ibankserverclientprotocolknower.h"

#include "network/ssl/ssltcpclient.h"

class SslTcpClientAndBankServerProtocolKnower : public IBankServerClientProtocolKnower
{
    Q_OBJECT
public:
    explicit SslTcpClientAndBankServerProtocolKnower(QObject *parent = 0);
protected:
    void myTransmit(IMessage *message);
private:
    SslTcpClient *m_SslTcpClient;
private slots:
    void handleConnectedAndEncrypted(QSslSocket *socket);
    void handleMessageReceivedFromRpcServerOverNetwork();
public slots:
    void init();
    void start();
    void stop();
};
#endif

#endif // SSLTCPCLIENTANDBANKSERVERPROTOCOLKNOWER_H
