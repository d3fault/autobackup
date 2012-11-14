#ifndef MULTISERVERHELLOER_H
#define MULTISERVERHELLOER_H

#include <QObject>
#include <QIODevice>
#include <QDataStream>
#include <QHash>

#include "ssltcpserver.h"

#define MAGIC_BYTE_SIZE 4 //if you change this, you also need to change the m_MagicExpected array to match

//this entire prototype is more about testing out magic robustity than hello'ing. hello'ing is fucking easy, but i've never done the robust magic shit i'm attempting (and plan to use in normal message use too, not just this hello shit)

struct ServerHelloStatus
{
    ServerHelloStatus() { m_ServerHelloState = AwaitingHello; m_CurrentMagicByteIndex = 0; m_MagicGot = false; m_Cookie.clear(); m_HasCookie = false; }
    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
    };
    ServerHelloState m_ServerHelloState;
    //ServerHelloState serverHelloState() { return m_ServerHelloState; }

    void setMagicGot(bool toSet) { m_MagicGot = toSet; }
    bool needsMagic() { return !m_MagicGot; }
    void setCookie(const QString &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    QString cookie() { if(!m_HasCookie) { m_Cookie = generateCookie(); /* after inventing the universe of course, that's implied */ m_HasCookie = true; } return m_Cookie; } //suddenly i am hungry

    static const quint8 m_MagicExpected[MAGIC_BYTE_SIZE] = { 'F', 'U', 'C', 'K' };

    quint8 m_CurrentMagicByteIndex;
    bool m_MagicGot;

    bool m_HasCookie;
    QString m_Cookie;
};

class MultiServerHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerHelloer(QObject *parent = 0);
    void startAll3Listening();
private:
    SslTcpServer *m_SslTcpServer;



    //still in hello phase
    QHash<QIODevice*, ServerHelloStatus*> m_ServerHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint16, QIODevice*> m_ClientsByCookie;
signals:
    void d(const QString &);
private slots:
    void handleNewClientConnected(QIODevice *newClient);
    void handleNewClientSentData();
};

#endif // MULTISERVERHELLOER_H
