#ifndef MULTISERVERCLIENTABSTRACTION_H
#define MULTISERVERCLIENTABSTRACTION_H

#include <QObject>
#include <QHash>
#include <QCryptographicHash>
#include <QQueue>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QLocalSocket>
#include <QDateTime> //debug(?)

#include "abstractserverconnection.h"

#include "ssltcpclient.h"

struct MultiServerClientsAbstractionArgs
{
    //for the client, each list is iterated and that's how many connections are set up... TODOoptimization would make sense to be able to add/remove connections later, but I like having my "initialize" able to take ALL of them and my start() require none. Flexible/future-proof

    //SSL
    QList<SslTcpClientArgs> SslTcpClientsArgs;

    //TCP
    QList<QPair<QHostAddress /*host*/,quint16/*port*/> > TcpClientsArgs;

    //LOCALSERVER
    QList<QString> LocalServersNames;
};

//ACTUALLY I THINK THE BELOW TODOreq is now false, but meh leaving just in case because my mind is double backing left and right all the fucking time. Feels great because it's the only way to progress... but feels bad because it means I have to be wrong quite often. Ok I sort of got into the whole "life" meaning of what I was saying and away from simply programming there...
//TODOreq: hmm actually not sure this makes any sense heh. It makes sense for a server to be multi-protocol... but doesn't really make any sense for a client to be multi-protocol (unless it has 'try other server' mechanisms built in'... but those should go at a different/higher level, not this one!) Still going to continue with this design out of sheer laziness :). Like I mean the protocol connection types can/should share a common base (THEY SHOULD IN QT ALREADY BUT DON'T FUCKING NOOBS ;-P), but they don't need to all live in one class like this (whereas in the server it makes sense for them to)
class MultiServerClientAbstraction : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerClientAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent);
    ~MultiServerClientAbstraction();
    void setupSocketSpecificDisconnectAndErrorSignaling(QIODevice *ioDeviceToClient, AbstractServerConnection *abstractClientConnection);
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
    AbstractServerConnection *potentialMergeCaseAsCookieIsSupplied_returning_oldConnection_ifMerge_or_ZERO_otherwise(AbstractServerConnection *newConnectionToPotentiallyMergeWithOld);
    void connectionDoneHelloing(AbstractServerConnection *abstractClientConnection);
    void appendDeadConnectionThatMightBePickedUpLater(AbstractServerConnection *abstractClientConnection);

    AbstractServerConnection *getSuitableClientConnectionNextInRoundRobinToUseForBroadcast_OR_Zero();
    void reportConnectionDestroying(AbstractServerConnection *connectionBeingDestroyed);
    void dontBroadcastTo(AbstractServerConnection *abstractClientConnection);
private:
    //SslTcpClient *sslTcpClient;
    //MultiServerClientAbstractionArgs m_MultiServerClientAbstractionArgs;

    AbstractServerConnection *handleNewConnectionToServer(QIODevice *newClient);
    QList<QIODevice*> m_ListOfIODevicesIgnoringConnectionAndHelloState; //TODOreq: use this to clear out stale/failed connections i guess. On the server it has a different means of organizing the connections

    QList<AbstractServerConnection*> m_ListOfConnectionsIgnoringHelloState;
    QList<AbstractServerConnection*> m_ListOfHelloedConnections;

    QQueue<AbstractServerConnection*> m_RoundRobinQueue;

    QList<AbstractServerConnection*> m_ListOfDeadConnectionsThatMightBePickedUpLater;

    inline AbstractServerConnection *getExistingConnectionUsingCookie__OrZero(quint32 cookie)
    {
        AbstractServerConnection *currentClientConnection;

        int connectionsCount = m_ListOfHelloedConnections.size();        
        for(int i = 0; i < connectionsCount; ++i)
        {
            currentClientConnection = m_ListOfHelloedConnections.at(i);
            if(currentClientConnection->cookie() == cookie)
            {
                return currentClientConnection;
            }
        }

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
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand()));
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt()));
    }
    static quint32 overflowingClientIdsUsedAsInputForMd5er;
    void refillRoundRobinFromHellodConnections();
    void setupQAbstractSocketSpecificErrorConnections(QAbstractSocket *abstractSocket, AbstractServerConnection *abstractClientConnection);
    void setupSslSocketSpecificErrorConnections(QSslSocket *sslSocket, AbstractServerConnection *abstractClientConnection);
    void setupQLocalSocketSpecificErrorConnections(QLocalSocket *localSocket, AbstractServerConnection *abstractClientConnection);
signals:
    void d(const QString &);
private slots:
    void handleConnectedToSslServer(QSslSocket* sslSocketToServer);
public slots:
    void initializeAndStartConnections(MultiServerClientsAbstractionArgs multiServerClientAbstractionArgs);
    void start();
    void stop();
};

#endif // MULTISERVERCLIENTABSTRACTION_H
