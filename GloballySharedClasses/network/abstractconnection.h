#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <QObject>

class AbstractConnection : public QObject
{
    Q_OBJECT
public:
    explicit AbstractConnection(QObject *parent = 0);

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
signals:
    
public slots:
    
};

#endif // ABSTRACTCONNECTION_H
