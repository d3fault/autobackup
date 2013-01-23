#ifndef ABSTRACTCLIENTCONNECTION_H
#define ABSTRACTCLIENTCONNECTION_H

#include <QObject>
#include <QIODevice>
#include <QDataStream>
#include <QCryptographicHash>
#include <QDateTime> //debug(?)

#include "networkmagic.h"
#include "bytearraymessagesizepeekerforiodevice.h"
#include "iprotocolknower.h"
#include "iprotocolknowerfactory.h"

class AbstractClientConnection : public QObject
{
	Q_OBJECT
public:
    explicit void AbstractClientConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory);
private:
    QByteArray m_TheMessageByteArray;
    QBuffer m_TheMessageBuffer;
    QDataStream m_TheMessageDataStream;

    QIODevice *m_IoDeviceToClient;
    QDataStream m_DataStreamToClient;

    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower;

    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched, //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
        DoneHelloing //TODOreq: put the connection in this state during the connect-to-other-slot phase and ADDITIONALLY, when detecting a reconnection, the provided cookie has to be able to look up a connection with the same cookie and that other connection (don't look up ourselves!) has to be in the DoneHelloing phase (or perhaps UglyDisconnectDetected state if/when it exists) for us to be able to... merge the connections. meh this seems pointless, probably is. Merging sounds easy and hard at the same time, idfk anymore
    };

    ServerHelloState m_ServerHelloState;
    NetworkMagic m_NetworkMagic;
    ByteArrayMessageSizePeekerForIODevice *m_IODevicePeeker;

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    quint32 cookie() { if(!m_HasCookie) { m_Cookie = generateUnusedCookie(); /* after inventing the universe of course, that's implied */ m_HasCookie = true; } return m_Cookie; } //suddenly i am hungry

    static quint32 overflowingClientIdsUsedAsInputForMd5er; //the datetime and a call to qrand() is added to make the overflowing irrelevant

    bool m_HasCookie;
    quint32 m_Cookie;

    static QList<AbstractClientConnection*> m_ListOfHelloedConnections;
    static AbstractClientConnection *getExistingConnectionUsingCookie__OrZero(quint32 cookie)
    {
        int connectionsCount = m_ListOfHelloedConnections.size();
        AbstractClientConnection *currentClientConnection;
        for(int i = 0; i < connectionsCount; ++i)
        {
            currentClientConnection = m_ListOfHelloedConnections.at(i);
            if(currentClientConnection->cookie() == cookie)
            {
                return currentClientConnection;
            }
        }
        return 0;
    }
    static quint32 generateUnusedCookie()
    {
        quint32 cookie;
        do
        {
            cookie = generateCookie();
        }
        while(getExistingConnectionUsingCookie__OrZero(cookie));
        return cookie;
    }
    static quint32 generateCookie()
    {
        QByteArray md5Input;
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand())); //no idea if this is a security issue, but could add some salt to this also (but then the salt has to be generated (which means it's salt has to be generated (etc)))
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt())); //for the time being, i'm just not going to give a shit about the truncation. lol /lazy
    }
    void potentialMergeCaseAsCookieIsSupplied();
signals:
    void d(const QString &);
public slots:
    void handleDataReceivedFromClient();
};

#endif // ABSTRACTCLIENTCONNECTION_H
