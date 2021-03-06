#ifndef ABSTRACTCLIENTCONNECTION_H
#define ABSTRACTCLIENTCONNECTION_H

#include "abstractconnection.h"
#include <QIODevice>
#include <QDataStream>
#include <QAbstractSocket>
#include <QLocalSocket>

#include "networkmagic.h"
#include "bytearraymessagesizepeekerforiodevice.h"

class MultiServerAbstraction;
class IProtocolKnower;
class IProtocolKnowerFactory;

class AbstractClientConnection : public AbstractConnection
{
	Q_OBJECT
public:
    explicit AbstractClientConnection(QIODevice *ioDeviceToClient, QObject *parent = 0);
    ~AbstractClientConnection();
    static void setMultiServerAbstraction(MultiServerAbstraction *multiServerAbstraction) { m_MultiServerAbstraction = multiServerAbstraction; }
    static void setProtocolKnowerFactory(IProtocolKnowerFactory *protocolKnowerFactory) { m_ProtocolKnowerFactory = protocolKnowerFactory; }
    //TODOcleanup: friend class the transmitMessage method to IProtocolKnower so it can be private
    //TODOreq: inline/implicit optimization mb idfk
    //TODOreq: this use of m_ConnectionGood is only for sending, but we should also use it when reading i think? But idk really I might design away the variable altogether, my brain is struggling (but I'm making progress) converging clean/ugly disconnect and merges :-D -- time to smoke some weed and watch a movie, I can only code so much per day when it's this complicated :-/. I liked thinking about that git submodule problem more because it was so much simpler. This codebase is getting fucking nuts. I wonder if I'll ever see it functioning. Guess I have to otherwise I won't see anything at all (suicide)

#if 0
    inline void streamDoneHelloingFromServerIntoMessageAboutToBeTransmitted(QByteArray *messageAboutToBeTransmitted)
    {
        //could have just had the caller of this do the streaming himself, but it seems more organized to have it here. Since it's inlined it makes no difference anyways...

        //turns out i need a QDS to wrap around the QBA so i'll do the above hack that was just mentioned as being less organized lmfao

                it's KIND OF less portable, but the calling class is generated ANYWAYS so it doesnt matter fuck the police. the MultiServerAbstraction and AbstractClientConnection is already pretty specific and dependent on Rpc Generator (the design surely isn't, but the impl seems to be), so this is probably OK for now

    }
#endif
    quint32 cookie();

    //right now when setMergeInProgress is called, it's when a matching cookie is seen and we have yet to see any socket errors etc. It's easy to 'guarantee' (except not) that a merge is in progress (the hello might still fail? we could still dc? idfk). However I still want to "MAYBE" do a merge whenever a socket error (unclean disconnect via makebad etc) is detected. It's more of like a setMergeMaybeInProgress though because we have no idea if they'll be back (for my systems, yes, it's a safe assumption that anyone that doens't cleanly disconnect will be back, but there's no way in hell i'd ever let my code assume that! needs to timeout and flush the queue after a period of no reconnect). basically i'm just trying to think while i type, what should i do inside makeConnectionBad with relation to the merge? I mean it's semantics but I still need to figure it out. This isn't necessarily right to call it a merge in progress, it's more like a potential merge in progress and the actual merge happens right after the new connections finishes hello'ing (TODOreq: make sure it only happens then)
    void setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(bool queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection);
    bool queueActionResponsesBecauseTheyMightBeReRequestedInNewConnection() { return m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection; }
    inline IProtocolKnower *protocolKnower() { return m_ProtocolKnower; }
protected:
    //bool m_ConnectionGood; -- pretty sure m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection does this job for me (TODOreq: make sure it's checked in all the right places, m_ConnectionGood was a last resort means, aka sloppy and perhaps never was even necessary)
private:
    AbstractClientConnection *m_OldConnectionToMergeOnto;
    void mergeNewIoDevice(QIODevice *newIoDeviceToClient);
    bool m_QueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection;

    QByteArray m_ReceivedMessageByteArray;
    QBuffer m_ReceivedMessageBuffer;
    QDataStream m_ReceivedMessageDataStream;

    QIODevice *m_IoDeviceToClient;
    //QDataStream m_DataStreamToClient;

    static MultiServerAbstraction *m_MultiServerAbstraction;
    static IProtocolKnowerFactory *m_ProtocolKnowerFactory;
    IProtocolKnower *m_ProtocolKnower; //TODOreq (pretty sure it lives on and only the IO device changes? I forget tbh rofl. It would still need to be deleted on shutdown/destruct etc tho): where is this deleted regularly? I am now looking for deleting it as per it being a part of an unneeded new connection (that is merging with and using an old connection's protocol knower), but I think this still needs to be deleted somewhere during regular flow

    //ServerHelloState m_ServerHelloState; -- so I guess instead of storing this as a member associated with the connection (and the server implicitly "knowing" what will come next), I want it to be a part of the protocol sent with each message (typically just "DoneHelloing" sent over and over). The reason for that is because I don't know how else I'd ever be able to "implicitly know" when to do a Disconnect/Goodbye! There wouldn't be a way to do DoneHelloing -> DisconnectGoodbye with my current (err, the currently coded but outdated (updating NOW)) design

    ByteArrayMessageSizePeekerForIODevice m_IODevicePeeker;

    void setCookie(const quint32 &cookie) { m_HasCookie = true; m_Cookie = cookie; }
    bool m_HasCookie;
    quint32 m_Cookie;
    void setupConnectionsToIODevice();
signals:
    void d(const QString &);
public slots:
    void makeConnectionBad();

    //these two slots make it not as abstract, but however they don't place a dependency on future socket types so who cares
    void makeConnectionBadIfNewQAbstractSocketStateSucks(QAbstractSocket::SocketState newState);
    void makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState newState);

    void handleDataReceivedFromClient();
};

#endif // ABSTRACTCLIENTCONNECTION_H
