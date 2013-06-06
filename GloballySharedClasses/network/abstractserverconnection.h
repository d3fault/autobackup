#ifndef ABSTRACTSERVERCONNECTION_H
#define ABSTRACTSERVERCONNECTION_H

#include <QObject>
#include <QIODevice>
#include <QDataStream>
#include <QAbstractSocket>
#include <QLocalSocket>

#include "networkmagic.h"
#include "bytearraymessagesizepeekerforiodevice.h"

class MultiServerClientAbstraction;
class IProtocolKnower;
class IProtocolKnowerFactory;

//TODOreq: merge guts from "ACC" to "ASC" (naming already done)
class AbstractServerConnection : public QObject
{
	Q_OBJECT
public:
    enum HelloMessageTypeFromServer
    {
        InvalidHelloStateFromServer = 0x0,
        WelcomeFromServer,
        OkStartSendingBroFromServer,
        DoneHelloingFromServer,
        DisconnectGoodbyeAcknowledgedPeaceOutBitchFromServer
    };

    explicit AbstractServerConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    ~AbstractServerConnection();
    static void setMultiServerAbstraction(MultiServerClientAbstraction *multiServerAbstraction) { m_MultiServerClientAbstraction = multiServerAbstraction; }
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory) { m_ProtocolKnowerFactory = protocolKnowerFactory; }
    inline void transmitMessage(QByteArray *message) { NetworkMagic::streamOutMagic(&m_DataStreamToClient); m_DataStreamToClient << *message; }
    quint32 cookie();

    void setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(bool queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection);
    bool queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection() { return m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection; }
    inline IProtocolKnower *protocolKnower() { return m_ProtocolKnower; }
private:
    AbstractServerConnection *m_OldConnectionToMergeOnto;    
    void mergeNewIoDevice(QIODevice *newIoDeviceToClient);
    bool m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection;

    QByteArray m_ReceivedMessageByteArray;
    QBuffer m_ReceivedMessageBuffer;
    QDataStream m_ReceivedMessageDataStream;

    QIODevice *m_IoDeviceToClient;
    QDataStream m_DataStreamToClient;

    static MultiServerClientAbstraction *m_MultiServerClientAbstraction;
    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower;
    enum HelloMessageTypeFromClient
    {
        InvalidHelloStateFromClient = 0x0,
        InitialHelloFromClient,
        ThankYouForWelcomingMeFromClient,
        DoneHelloingFromClient,
        DisconnectGoodbyeFromClient
    };

    NetworkMagic m_NetworkMagic;
    ByteArrayMessageSizePeekerForIODevice m_IODevicePeeker;

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    bool m_HasCookie;
    quint32 m_Cookie;
    void setupConnectionsToIODevice();
signals:
    void d(const QString &);
public slots:
    void makeConnectionBad();

    void makeConnectionBadIfNewQAbstractSocketStateSucks(QAbstractSocket::SocketState newState);
    void makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState newState);

    void handleDataReceivedFromClient();
};

#endif // ABSTRACTSERVERCONNECTION_H
