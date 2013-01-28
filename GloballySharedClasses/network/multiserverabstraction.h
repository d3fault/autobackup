#ifndef MULTISERVERABSTRACTION_H
#define MULTISERVERABSTRACTION_H

#include <QObject>
#include <QHash>
#include <QCryptographicHash>
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
class MultiServerAbstraction : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent);
    ~MultiServerAbstraction();
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
    void potentialMergeCaseAsCookieIsSupplied(AbstractClientConnection *newConnectionToPotentiallyMergeWithOld);
    void connectionDoneHelloing(AbstractClientConnection *abstractClientConnection);
    //void sendMessage(QByteArray *message, quint32 clientId);
private:
    bool m_BeSslServer, m_BeTcpServer, m_BeLocalServer;
    SslTcpServer *m_SslTcpServer;
    inline void deletePointersAndSetEachFalse() { delete m_SslTcpServer; /* TODOreq: delete Tcp/Local */ m_BeSslServer = false; m_BeTcpServer = false; m_BeLocalServer = false; }


    //still in hello phase
    QList<AbstractClientConnection*> m_ClientConnections;

    void handleNewClientConnected(QIODevice *newClient);

    //TODOreq: Still no clue what to do when I receive the same cookie twice... or if it's good enough security [since I'm using SSL?]...
    QList<AbstractClientConnection*> m_ListOfHelloedConnections;
    inline AbstractClientConnection *getExistingConnectionUsingCookie__OrZero(quint32 cookie)
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
    static inline quint32 generateCookie()
    {
        QByteArray md5Input;
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand())); //no idea if this is a security issue, but could add some salt to this also (but then the salt has to be generated (which means it's salt has to be generated (etc)))
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt())); //for the time being, i'm just not going to give a shit about the truncation. lol /lazy
    }
    static quint32 overflowingClientIdsUsedAsInputForMd5er; //the datetime and a call to qrand() is added to make the overflowing irrelevant
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
