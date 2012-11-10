#ifndef MULTISERVERANDHELLOER_H
#define MULTISERVERANDHELLOER_H

#include <QObject>
#include <QHash>

#include "network/ssl/ssltcpserver.h"

struct ServerHelloStatus
{
    ServerHelloStatus() { m_ServerHelloState = AwaitingHello; }
    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
    };
    ServerHelloState m_ServerHelloState;
    ServerHelloState serverHelloState() { return m_ServerHelloState; }
};

class MultiServerAndHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerAndHelloer(QObject *parent = 0, bool sslServer, bool tcpServer, bool localSocketServer);
private:
    bool m_BeSslServer;
    bool m_BeTcpServer;
    bool m_BeLocalSocketServer;

    void initSslServer();
    void initTcpServer();
    void initLocalSocketServer();

    void startSslServer();

    SslTcpServer *m_SslTcpServer;



    //still in hello phase
    QHash<QIODevice*, ServerHelloStatus*> m_ServerHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint16, QIODevice*> m_ClientsByCookie;
signals:
    void d(const QString&);
public slots:
    void initialize();
    void startListening();

    void handleNewClientConnected(QIODevice *newClient);
    void handleNewClientSentData();
};

#endif // MULTISERVERANDHELLOER_H
