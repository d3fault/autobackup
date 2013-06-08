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
//^^^^_IDEALLY_ but it would be hard as fuck: if we lose connection to the server and can't re-establish it (which would be same io device, right?), we should pass the http user details (he who made the request), as well as the request itself, over to one of our neighbor machines who is running another of the same instance of our "smart client". He has a different internet link and should try to establish a connection to the same server (but if he can't that means the server is down and the smart client who passed his shit to us is *PROBABLY* ok (not a guarantee though). If that server doesn't work then yea we try another (but this could still lead to data failure as we don't know that the original didn't receive it and process it, regardless of it's internet being down). If we are able to establish a connection with the same server (new IO device from server's perspective -- which is how I apparently coded the reconnect/merge to begin with), then we can 'pick up where the other smart client left off' or some shit. So incredibly confusing idk wtf I'm getting at...
//^^^If we, or our
//wait shouldn't the first smart client first try another server before passing on to neighbor smart client (other machine)? FUuck
//....but what i was saying is that any of the servers should be able to "steal" the other server's unfinished request (or just re-route it's response when it comes!). Like if that server's internet link went down but the server itself is fine and of course we have a LAN connection to it so we can still talk to it...
//^^^^Wouldn't a mass transparent "auto connection re-route" type solution be better? The server with the downed internet link just uses another server's? OR: is that exactly what Rpc Generator is turning into? Hahahahaha I have no idea my imagination gets the best of me and it's actually NOT a good thing at this point in my life. Need to launch ASEP. KISS.
//TODOreq: need to see what an actual physical disconnect (or perhaps "app KILL" will suffice?) looks like (debugging etc) and what happens when the link becomes reconnected. Does the OS/Qt handle the reconnect itself or do I have to detect it and do it myself (Also just realized I probably need timers/logic for both the connect and reconnect (ideally they'd use the same code))? I'm actually guessing it will be the SAME io device after the reconnect so my server merge code is probably retardedly overkill (but would come in handy when picking up where left off FROM A DIFFERENT "smart client" (should the one we were communicating fail (it's link, it's software, etc)))
class AbstractServerConnection : public AbstractConnection
{
	Q_OBJECT
public:
    explicit AbstractServerConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    ~AbstractServerConnection();
    static void setMultiServerClientAbstraction(MultiServerClientAbstraction *multiServerAbstraction) { m_MultiServerClientAbstraction = multiServerAbstraction; }
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory) { m_ProtocolKnowerFactory = protocolKnowerFactory; }

    quint32 cookie();

    inline IProtocolKnower *protocolKnower() { return m_ProtocolKnower; }
private:
    QByteArray m_ReceivedMessageByteArray;
    QBuffer m_ReceivedMessageBuffer;
    QDataStream m_ReceivedMessageDataStream;

    QIODevice *m_IoDeviceToClient;

    static MultiServerClientAbstraction *m_MultiServerClientAbstraction;
    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower;

    ByteArrayMessageSizePeekerForIODevice m_IODevicePeeker;

    void setCookie(const quint32 &cookie) { m_Cookie = cookie; }
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
