#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <QObject>
#include <QDataStream>

#include "networkmagic.h"

/*
    Magic
        Hello State
            "Rpc Message Header" (MessageId, Generic Rpc Message Type, Rpc Service Specific Message Type)
                Rpc Message

*/

//TODOoptional: can probably move a lot of the functionality from my inheriters into this class... but don't give a fuck for now...
class AbstractConnection : public QObject
{
    Q_OBJECT
public:
    explicit AbstractConnection(QIODevice *ioDeviceToPeer, QObject *parent = 0);

    enum HelloMessageTypeFromClient
    {
        InvalidHelloStateFromClient = 0x0,
        InitialHelloFromClient,
        ThankYouForWelcomingMeFromClient, //cookie optionally received with hello. if not, we generate and send with welcome. send cookie with welcome either way
        //OkStartSendingBroDispatched, //cookie receive mandatory and must match welcome. ok start sending bro dispatched should only be sent after we have connected up to the server IMPL/main-business one (and disconnected from our hello'er one). client does not actually start sending until he receives "ok start sending bro"
        DoneHelloingFromClient, //TODOreq (what bitch? we don't use states no mo!): put the connection in this state during the connect-to-other-slot phase and ADDITIONALLY, when detecting a reconnection, the provided cookie has to be able to look up a connection with the same cookie and that other connection (don't look up ourselves!) has to be in the DoneHelloing phase (or perhaps UglyDisconnectDetected state if/when it exists) for us to be able to... merge the connections. meh this seems pointless, probably is. Merging sounds easy and hard at the same time, idfk anymore
        DisconnectGoodbyeFromClient //TODOreq: as of this writing all I've done is put in this enum value. It may need to involve "disconnect REQUESTing", but I am pretty sure that is done at the generic rpc "status" level and not the hello level (here). A DisconnectRequested (Rpc Message Type) will be a DoneHelloing hello type.... I THINK but am unsure atm. Shit is in progress atm
    };
    enum HelloMessageTypeFromServer
    {
        InvalidHelloStateFromServer = 0x0,
        WelcomeFromServer,
        OkStartSendingBroFromServer,
        DoneHelloingFromServer,
        DisconnectGoodbyeFromServerAcknowledgedButDontGoAnywhereYet, //TODOreq: should client then say "ok  thx i'll be waiting" in response to this?
        DisconnectGoodbyeFromServerAcknowledgedAndFinishedSeeYa
    };

    //TODOreq (think it's done but leaving in for server->client unfinished code): Need to have a QBA that encapsulates both the "DoneHelloing" state and also the "actual message" (which is passed in full below). Something like: QBA[HelloState,ActualMessage_QBA-also_OnlyIfHelloStateIsDoneHelloing]. The only downside to this is that we have introduced another quint32 for another QBA! Is there some way I can combine them without going batshit insane? Hmm just realized this is for server -> client stuff, which I'm not even completely balls deep into yet. It is still relevant but also like the "opposite side of the coin" too. In streamToByteArrayAndTransmit I could call a method in this to stream the HelloState just before streaming the message contents themselves to get rid of the added quint32 for the extra qba, but what will it be? DoneHelloing? I'm starting to think I don't need an "Ok start sending bro" because that is essentially the server telling the client "DoneHelloing". Do I have client -> server and server -> client statuses mixed up unintentionally? My brain hurts and I think yes. Either way it's a TODOreq: that all of the non-DoneHelloing cases need to ALSO manually stream the hello status where appropriate (in the switch statement down below), similar to how they also manually stream magic themselves...
    inline void transmitMessage(QByteArray *message) { /*if(m_ConnectionGood) {*/ NetworkMagic::streamOutMagic(&m_DataStreamToPeer); m_DataStreamToPeer << *message; /*}*/ }
protected:
    QDataStream m_DataStreamToPeer;
    NetworkMagic m_NetworkMagic;
};

#endif // ABSTRACTCONNECTION_H
