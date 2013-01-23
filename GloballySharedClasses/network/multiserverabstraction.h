#ifndef MULTISERVERABSTRACTION_H
#define MULTISERVERABSTRACTION_H

#include <QObject>
#include <QHash>

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
    void sendMessage(QByteArray *message, quint32 clientId);
private:
    bool m_BeSslServer, m_BeTcpServer, m_BeLocalServer;
    SslTcpServer *m_SslTcpServer;
    inline void deletePointersAndSetEachFalse() { delete m_SslTcpServer; /* TODOreq: delete Tcp/Local */ m_BeSslServer = false; m_BeTcpServer = false; m_BeLocalServer = false; }


    //still in hello phase
    QList<AbstractClientConnection*> m_ClientConnections;
    QHash<QIODevice*, ServerHelloStatus*> m_ServerHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint32, QIODevice*> m_ClientsByCookie;

    void handleNewClientConnected(QIODevice *newClient);
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
