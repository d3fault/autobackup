#ifndef ABSTRACTCLIENTCONNECTION_H
#define ABSTRACTCLIENTCONNECTION_H

#include <QObject>
#include <QIODevice>
#include <QDataStream>

#include "networkmagic.h"
#include "bytearraymessagesizepeekerforiodevice.h"

class MultiServerAbstraction;
class IProtocolKnower;
class IProtocolKnowerFactory;

class AbstractClientConnection : public QObject
{
	Q_OBJECT
public:
    explicit AbstractClientConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    ~AbstractClientConnection();
    static void setMultiServerAbstraction(MultiServerAbstraction *multiServerAbstraction) { m_MultiServerAbstraction = multiServerAbstraction; }
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory) { m_ProtocolKnowerFactory = protocolKnowerFactory; }

    //TODOcleanup: friend class the transmitMessage method to IProtocolKnower so it can be private
    //TODOreq: inline/implicit optimization mb idfk
    inline void transmitMessage(QByteArray *message) { NetworkMagic::streamOutMagic(&m_DataStreamToClient); m_DataStreamToClient << *message; }
    quint32 cookie();
    void setQueueActionResponses(bool queueActionResponses) { m_QueueActionResponses = queueActionResponses; }
    bool queueActionResponses() { return m_QueueActionResponses; }
    inline IProtocolKnower *protocolKnower() { return m_ProtocolKnower; }
private:
    AbstractClientConnection *m_OldConnectionToMergeOnto;
    void mergeNewIoDevice(QIODevice *newIoDeviceToClient);
    bool m_QueueActionResponses;

    QByteArray m_ReceivedMessageByteArray;
    QBuffer m_ReceivedMessageBuffer;
    QDataStream m_ReceivedMessageDataStream;

    QIODevice *m_IoDeviceToClient;
    QDataStream m_DataStreamToClient;

    static MultiServerAbstraction *m_MultiServerAbstraction;
    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower; //TODOreq: where is this deleted regularly? I am now looking for deleting it as per it being a part of an unneeded new connection (that is merging with and using an old connection's protocol knower), but I think this still needs to be deleted somewhere during regular flow

    enum ServerHelloState
    {
        HelloFailed,
        AwaitingHello,
        HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched, //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
        DoneHelloing //TODOreq: put the connection in this state during the connect-to-other-slot phase and ADDITIONALLY, when detecting a reconnection, the provided cookie has to be able to look up a connection with the same cookie and that other connection (don't look up ourselves!) has to be in the DoneHelloing phase (or perhaps UglyDisconnectDetected state if/when it exists) for us to be able to... merge the connections. meh this seems pointless, probably is. Merging sounds easy and hard at the same time, idfk anymore
    };

    ServerHelloState m_ServerHelloState;
    NetworkMagic m_NetworkMagic;
    ByteArrayMessageSizePeekerForIODevice m_IODevicePeeker;

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    bool m_HasCookie;
    quint32 m_Cookie;
    void setupConnections();
signals:
    void d(const QString &);
public slots:
    void handleDataReceivedFromClient();
};

#endif // ABSTRACTCLIENTCONNECTION_H
