#ifndef MULTISERVERABSTRACTION_H
#define MULTISERVERABSTRACTION_H

#include <QObject>
#include <QHash>
#include <QCryptographicHash>
#include <QQueue>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QLocalSocket>
#include <QDateTime> //debug(?)

#include "abstractclientconnection.h"

#include "ssl/ssltcpserver.h"

struct MultiServerAbstractionArgs
{
    MultiServerAbstractionArgs() { m_SslTcpEnabled = false; m_TcpEnabled = false; m_LocalServerEnabled = false; }

    //SSL
    SslTcpServerArgs m_SslTcpServerArgs;
    bool m_SslTcpEnabled;

    //TCP
    QHostAddress m_TcpHostAddress;
    quint16 m_TcpPort;
    bool m_TcpEnabled;

    //LOCALSERVER
    QString m_LocalServerName;
    bool m_LocalServerEnabled;
};
//TODOreq: not a huge priority, but there should be a periodic timeout to clear out old incomplete hellos. if for some reason the hello didn't finish and it's still in any one of the states, it will remain their indefinitely (and theoretically/potentially leak memory). obviously also applies to client as well
//TODOreq: if the connection we're merging into wasn't finished hello'ing we pretty much just treat it like a new connection. i am afraid there would be errors where messages could get through without hello phase being completed, but idfk tbh
class MultiServerAbstraction : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent);
    ~MultiServerAbstraction();
    void setupSocketSpecificDisconnectAndErrorSignaling(QIODevice *ioDeviceToClient, AbstractClientConnection *abstractClientConnection);
    inline quint32 generateUnusedCookie()
    {
        quint32 cookie;
        do
        {
            cookie = generateCookie();
        }
        while(getExistingConnectionUsingCookie__OrZero(cookie));
        return cookie;
    }
    AbstractClientConnection *potentialMergeCaseAsCookieIsSupplied_returning_oldConnection_ifMerge_or_ZERO_otherwise(AbstractClientConnection *newConnectionToPotentiallyMergeWithOld);
    void connectionDoneHelloing(AbstractClientConnection *abstractClientConnection);
    void appendDeadConnectionThatMightBePickedUpLater(AbstractClientConnection *abstractClientConnection);
    //void sendMessage(QByteArray *message, quint32 clientId);

    //void roundRobinBroadcastBecauseClientNotifiesNeighbors(IMess pendingBalanceDetectedMessage);
    AbstractClientConnection *getSuitableClientConnectionNextInRoundRobinToUseForBroadcast_OR_Zero();
    void reportConnectionDestroying(AbstractClientConnection *connectionBeingDestroyed);
    void dontBroadcastTo(AbstractClientConnection *abstractClientConnection);
private:
    bool m_BeSslServer, m_BeTcpServer, m_BeLocalServer;
    SslTcpServer *m_SslTcpServer;
    inline void deletePointersIfNotZeroAndSetEachEnabledToFalse() { if(m_SslTcpServer) { delete m_SslTcpServer; m_SslTcpServer = 0; } /* TODOreq: delete Tcp/Local */ m_BeSslServer = false; m_BeTcpServer = false; m_BeLocalServer = false; }

    AbstractClientConnection *handleNewClientConnected(QIODevice *newClient);

    //all connections, regardless of hello state
    QList<AbstractClientConnection*> m_ListOfConnectionsIgnoringHelloState; //TODOreq: wtf is the point of this? i append to it and remove from it.. but never do anything else with it. So I can probably delete it!?!?

    //only hello'd connections
    QList<AbstractClientConnection*> m_ListOfHelloedConnections;

    QQueue<AbstractClientConnection*> m_RoundRobinQueue; //this class manages access to it and makes sure it is not empty (lazy fill on first read)

    QList<AbstractClientConnection*> m_ListOfDeadConnectionsThatMightBePickedUpLater;

    inline AbstractClientConnection *getExistingConnectionUsingCookie__OrZero(quint32 cookie)
    {
        AbstractClientConnection *currentClientConnection;

        //First check all hello'd connections for the cookie
        int connectionsCount = m_ListOfHelloedConnections.size();        
        for(int i = 0; i < connectionsCount; ++i)
        {
            currentClientConnection = m_ListOfHelloedConnections.at(i);
            if(currentClientConnection->cookie() == cookie)
            {
                return currentClientConnection;
            }
        }

        //Then check all the dead connections too. We don't want to accidentally assign one that might reconnect with same (but still "is" different connection altogether) cookie
        connectionsCount = m_ListOfDeadConnectionsThatMightBePickedUpLater.size();
        for(int i = 0; i < connectionsCount; ++i)
        {
            currentClientConnection = m_ListOfDeadConnectionsThatMightBePickedUpLater.at(i);
            if(currentClientConnection->cookie() == cookie)
            {
                return currentClientConnection;
            }
        }

        return 0;
    }
    static inline quint32 generateCookie()
    {
        QByteArray md5Input;
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand())); //no idea if this is a security issue, but could add some salt to this also (but then the salt has to be generated (which means it's salt has to be generated (etc)))
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt())); //for the time being, i'm just not going to give a shit about the truncation. lol /lazy
    }
    static quint32 overflowingClientIdsUsedAsInputForMd5er; //the datetime and a call to qrand() is added to make the overflowing irrelevant
    void refillRoundRobinFromHellodConnections();
    void setupQAbstractSocketSpecificErrorConnections(QAbstractSocket *abstractSocket, AbstractClientConnection *abstractClientConnection);
    void setupSslSocketSpecificErrorConnections(QSslSocket *sslSocket, AbstractClientConnection *abstractClientConnection);
    void setupQLocalSocketSpecificErrorConnections(QLocalSocket *localSocket, AbstractClientConnection *abstractClientConnection);
signals:
    void d(const QString &);
private slots:
    void handleNewSslClientConnected(QSslSocket* newSslClient);
public slots:
    void initialize(MultiServerAbstractionArgs multiServerAbstractionArgs);
    void start();
    void stop();
};

#endif // MULTISERVERABSTRACTION_H
