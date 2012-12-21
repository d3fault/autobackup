#ifndef MULTISERVERABSTRACTION_H
#define MULTISERVERABSTRACTION_H

#include <QObject>
#include <QHash>

#include "networkmagic.h"
#include "bytearraymessagesizepeekerforiodevice.h"

#include "ssl/ssltcpserver.h"

class IMultiServerBusiness
{
protected:
    void messageReceived(QByteArray *message, quint32 clientId)=0;
};
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

struct ServerHelloStatus
{
    ServerHelloStatus() { m_ServerHelloState = AwaitingHello; m_HasCookie = false; }
    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched, //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
        DoneHelloing
    };
    ServerHelloState m_ServerHelloState;
    //ServerHelloState serverHelloState() { return m_ServerHelloState; }

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    quint32 cookie() { if(!m_HasCookie) { m_Cookie = generateCookie(); /* after inventing the universe of course, that's implied */ m_HasCookie = true; } return m_Cookie; } //suddenly i am hungry

    static quint32 overflowingClientIdsUsedAsInputForMd5er; //the datetime element is added to make the overflowing irrelevant

    bool m_HasCookie;
    quint32 m_Cookie;

    NetworkMagic m_NetworkMagic;
    ByteArrayMessageSizePeekerForIODevice *m_IODevicePeeker;

    static quint32 generateCookie()
    {
        QByteArray md5Input;
        md5Input.append(QString::number(++overflowingClientIdsUsedAsInputForMd5er) + QDateTime::currentDateTime().toString() + QString::number(qrand())); //no idea if this is a security issue, but could add some salt to this also (but then the salt has to be generated (which means it's salt has to be generated (etc)))
        return ((quint32)(QCryptographicHash::hash(md5Input, QCryptographicHash::Md5).toUInt())); //for the time being, i'm just not going to give a shit about the truncation. lol /lazy
    }
};
class MultiServerAbstraction : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerAbstraction(IMultiServerBusiness *serverBusiness, QObject *parent = 0);
    ~MultiServerAbstraction();
    void sendMessage(QByteArray *message, quint32 clientId);
private:
    IMultiServerBusiness *m_ServerBusiness;
    bool m_BeSslServer, m_BeTcpServer, m_BeLocalServer;
    SslTcpServer *m_SslTcpServer;
    inline void deletePointersAndSetEachFalse() { delete m_SslTcpServer; /* TODOreq: delete Tcp/Local */ m_BeSslServer = false; m_BeTcpServer = false; m_BeLocalServer = false; }


    //still in hello phase
    QHash<QIODevice*, ServerHelloStatus*> m_ServerHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint32, QIODevice*> m_ClientsByCookie;

    void handleNewClientConnected(QIODevice *newClient);

    QDataStream m_NetworkStreamToClient;
    QByteArray m_TheMessage;
signals:
    void d(const QString &);
private slots:
    void handleNewSslClientConnected(QSslSocket* newSslClient);
    void handleNewClientSentData();
public slots:
    void initialize(MultiServerAbstractionArgs multiServerAbstractionArgs);
    void start();
    void stop();
};

#endif // MULTISERVERABSTRACTION_H
