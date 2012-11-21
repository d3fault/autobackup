#ifndef MULTICLIENTHELLOER_H
#define MULTICLIENTHELLOER_H

#include <QObject>

#include "ssltcpclient.h"
#include "../ServerClientShared/networkmagic.h"
#include "../ServerClientShared/bytearraymessagesizepeekerforiodevice.h"

struct ClientHelloStatus
{
    ClientHelloStatus() { m_ClientHelloState = AwaitingConnection; }
    enum ClientHelloState
    {
        HelloFailed,
        AwaitingConnection,
        HelloDispatchedAwaitingWelcome,
        WelcomeReceivedThankYouForWelcomingMeSentAwaitingOkStartSendingBro
    };
    ClientHelloState m_ClientHelloState;
    NetworkMagic m_NetworkMagic;
};

class MultiClientHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiClientHelloer(QObject *parent = 0);
    ~MultiClientHelloer();
    void addSslClient();
private:
    //still in connecting or hello phase
    QHash<QIODevice*, ClientHelloStatus*> m_ClientHelloStatusesByIODevice;

    //done with hello phase. our business sends us a message to send and an id to send it to and we take care of the rest
    QHash<quint32, QIODevice*> m_ConnectionsByCookie;
signals:
    void d(const QString &);
private slots:
    void handleClientFinishedInitialConnectPhaseSoStartHelloing(QIODevice *newClient);
    void handleServerSentUsData();
};

#endif // MULTICLIENTHELLOER_H
