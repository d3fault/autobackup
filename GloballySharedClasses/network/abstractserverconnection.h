#ifndef ABSTRACTSERVERCONNECTION_H
#define ABSTRACTSERVERCONNECTION_H

#include "abstractconnection.h"
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
//TODOreq: client reconnect (new io device i guess?). it has to be the same server though (unless i change my design drastically so that cookies are shared among servers and their shared "data set" (dht))... and also I need to re-request Action Requests that were sent to the pre-reconnect io device... perhaps changing the enum stating that i'm re-requesting them but i forget how/what tbh
class AbstractServerConnection : public AbstractConnection
{
	Q_OBJECT
public:
    explicit AbstractServerConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    ~AbstractServerConnection();
    static void setMultiServerClientAbstraction(MultiServerClientAbstraction *multiServerAbstraction) { m_MultiServerClientAbstraction = multiServerAbstraction; }
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory) { m_ProtocolKnowerFactory = protocolKnowerFactory; }
    inline void transmitMessage(QByteArray *message) { NetworkMagic::streamOutMagic(&m_DataStreamToServer); m_DataStreamToServer << *message; }
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
    QDataStream m_DataStreamToServer;

    static MultiServerClientAbstraction *m_MultiServerClientAbstraction;
    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower;

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

    void handleDataReceivedFromServer();
};

#endif // ABSTRACTSERVERCONNECTION_H
