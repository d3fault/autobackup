#ifndef MULTISERVERHELLOER_H
#define MULTISERVERHELLOER_H

#include <QObject>
#include <QIODevice>
#include <QDataStream>
#include <QHash>
#include <QCryptographicHash>
#include <QDateTime>

#include "ssltcpserver.h"
#include "../ServerClientShared/networkmagic.h"
#include "../ServerClientShared/bytearraymessagesizepeekerforiodevice.h"

//this entire prototype is more about testing out magic robustity than hello'ing. hello'ing is fucking easy, but i've never done the robust magic shit i'm attempting (and plan to use in normal message use too, not just this hello shit)

struct ServerHelloStatus
{
    ServerHelloStatus() { m_ServerHelloState = AwaitingHello; m_HasCookie = false; }
    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
    };
    ServerHelloState m_ServerHelloState;
    //ServerHelloState serverHelloState() { return m_ServerHelloState; }

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    quint32 cookie() { if(!m_HasCookie) { m_Cookie = generateCookie(); /* after inventing the universe of course, that's implied */ m_HasCookie = true; } return m_Cookie; } //suddenly i am hungry

    static quint32 overflowingClientIdsUsedAsInputForMd5er; //the datetime element is added to make the overflowing irrelevant

    bool m_HasCookie;
    quint32 m_Cookie;

    NetworkMagic m_NetworkMagic;

    static quint32 generateCookie()
    {
        QByteArray md5Input;
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand())); //no idea if this is a security issue, but could add some salt to this also (but then the salt has to be generated (which means it's salt has to be generated (etc)))
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt()));
    }
};

class MultiServerHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerHelloer(QObject *parent = 0);
    ~MultiServerHelloer();
    void startAll3Listening();
private:
    SslTcpServer *m_SslTcpServer;



    //still in hello phase
    QHash<QIODevice*, ServerHelloStatus*> m_ServerHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint32, QIODevice*> m_ClientsByCookie;
signals:
    void d(const QString &);
    void connectionHasBeenHelloedAndIsReadyForAction(QIODevice*, quint32);
private slots:
    void handleNewClientConnected(QIODevice *newClient);
    void handleNewClientSentData();
};

#endif // MULTISERVERHELLOER_H
