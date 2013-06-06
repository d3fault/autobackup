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

#include "ssl/ssltcpclient.h"

struct MultiServerClientAbstractionArgs
{
    MultiServerClientAbstractionArgs() { m_SslTcpEnabled = false; m_TcpEnabled = false; m_LocalServerEnabled = false; }

    //SSL
    SslTcpServerClientArgs m_SslTcpServerClientArgs; //TODOreq: ssl server shit shouldn't use these custom args objects, only our multi server abstraction code should (unless the ssl server shit becomes specific only to multi server abstraction (which it isn't or shouldn't be (if it is, i'll copy/paste un-merge the two... for future uses)))
    bool m_SslTcpEnabled;

    //TCP
    QHostAddress m_TcpHostAddress;
    quint16 m_TcpPort;
    bool m_TcpEnabled;

    //LOCALSERVER
    QString m_LocalServerName;
    bool m_LocalServerEnabled;
};
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
    bool m_BeSslClient, m_BeTcpServer, m_BeLocalServer;
    SslTcpClient *m_SslTcpClient;
    inline void deletePointersIfNotZeroAndSetEachEnabledToFalse() { if(m_SslTcpClient) { delete m_SslTcpClient; m_SslTcpClient = 0; } /* TODOreq: delete Tcp/Local */ m_BeSslClient = false; m_BeTcpServer = false; m_BeLocalServer = false; }

    AbstractServerConnection *handleNewClientConnected(QIODevice *newClient);
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
    void handleNewSslClientConnected(QSslSocket* newSslClient);
public slots:
    void initialize(MultiServerClientAbstractionArgs multiServerClientAbstractionArgs);
    void start();
    void stop();
};

#endif // MULTISERVERCLIENTABSTRACTION_H
