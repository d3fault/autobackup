#ifndef MULTICLIENTHELLOER_H
#define MULTICLIENTHELLOER_H

#include <QObject>

#include "ssltcpclient.h"
#include "../ServerClientShared/networkmagic.h"
#include "../ServerClientShared/bytearraymessagesizepeekerforiodevice.h"

struct ClientHelloStatus
{
    ClientHelloStatus() { m_ClientHelloState = AwaitingConnection; m_Cookie = 0; }
    enum ClientHelloState
    {
        HelloFailed,
        AwaitingConnection,
        HelloDispatchedAwaitingWelcome,
        WelcomeReceivedThankYouForWelcomingMeSentAwaitingOkStartSendingBro
    };
    ClientHelloState m_ClientHelloState;
    NetworkMagic m_NetworkMagic;
    quint32 m_Cookie;
    ByteArrayMessageSizePeekerForIODevice *m_IODevicePeeker;
};

class MultiClientHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiClientHelloer(QObject *parent = 0);
    ~MultiClientHelloer();
    void addSslClient();
private:
    void clientFinishedInitialConnectPhaseSoStartHelloing(QIODevice *newClient);

    //still in connecting or hello phase
    QHash<QIODevice*, ClientHelloStatus*> m_ClientHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint32, QIODevice*> m_ConnectionsByCookie;
signals:
    void d(const QString &);
    void connectionHasBeenHelloedAndIsReadyForAction(QIODevice*, quint32);
private slots:
    void handleSslConnectionEstablished(QSslSocket *sslConnection);
    void handleServerSentUsData();
};

#endif // MULTICLIENTHELLOER_H
