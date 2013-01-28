#include "multiserverabstraction.h"

quint32 MultiServerAbstraction::overflowingClientIdsUsedAsInputForMd5er = 0;

//TODOreq: no idea where this belongs, but make sure not to delete an IProtocolKnower that still has messages that our rigged for delivery() back to it that are still pending in the business. deleting it with messages still in the business would result in the silent failure of the delivery of the messages to the (new?) IProtocolKnower. Not sure if/how/where this factors in, but it probably has something to do with merging? However I think merging means we keep the protocol knower and swap out the IO device? idfk [yet].

MultiServerAbstraction::MultiServerAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent)
    : QObject(parent), m_SslTcpServer(0) /* etc tcp local */
{
    AbstractClientConnection::setMultiServerAbstraction(this);
    AbstractClientConnection::setProtocolKnowerFactory(protocolKnowerFactory);

    deletePointersAndSetEachFalse();
}
MultiServerAbstraction::~MultiServerAbstraction()
{
    //TODOreq: flush? Perhaps it's implied when I do the delete? idfk
    deletePointersAndSetEachFalse();
}
void MultiServerAbstraction::potentialMergeCaseAsCookieIsSupplied(AbstractClientConnection *newConnectionToPotentiallyMergeWithOld)
{
    //At this point, our cookie is set with the client-supplied one, but we are not done with the hello phase (we are right int he middle of it!)... so we are not on m_ListOfHelloedConnections (which means we can check against it)

    AbstractClientConnection *oldConnectionToMergeWithMaybe = getExistingConnectionUsingCookie__OrZero(newConnectionToPotentiallyMergeWithOld->cookie());
    if(oldConnectionToMergeWithMaybe)
    {
        //merge. fuck, this seems like it should be a lot more complicated than a toMergeWithMaybe.setIoDevice(this.getIODevice), followed by a deletion of 'this' (deleteLater methinks?). Oh and I'd have to do the slot stuff too and also delete the original iodevice being overwritten???? I wonder if it's implicitly shared and has other references? Is this a security issue letting clients easily override/merge-with each other? It ultimately depends on the ssl cert being leaked i guess, or if you are dumb and choose TCP over WAN (in which case your shit can be hacked anyways~, even if you use a more elaborate username/password scheme). cert comrpomise means machine compromise means username/password compromise means fuck doing it (username/password) to begin with? I'm getting way away from the point. How the fuck do I merge two connections? This is where I keep getting stuck. I have to factor in shit like pending messages. Note that the cookie scheme is not a security measure, just an identification measure. It abstracts the connection types, because I can't use like IP address and shit for identification
        //also not sure if the server abstraction or myself should do the merging. i know one thing: IT DOESN'T FUCKING MATTER. _CHOOOOOOSE_
        //but, before i CHOOOOOOSE, figure out the fucking design of the merging! gah! brain is exploding over and over and over. there are so many places that a connection can go wrong, and i have to think of the case that accounts for all of them? or code for each separately? i can't even think about it properly. i don't know what the fuck to do. Wouldn't it involve the contents of clientshelper seeing as that's who stores the pending messages? am i going to make clientshelper an instance-per-connection type class as an optimization for accessing the m_Pending* lists, so that each connection has it's own list (along with each message type having it's own)???

        //TODOoptional: as a security measure, we could attempt to ask the OLD iodevice with the matching cookie if it is still there and if it is ok to be overwritten/whatever. if it responds at all, we pretty much want to NOT merge. if it is even able to respond, we should not be merging to begin with? i'd probably need a hard timeout for that asking


        //maybe this will become clear to me once i have the pending shit coded and working???? hmmm, since it depends on it, that actually sounds very likely

        //TODOreq: now that I have pending shit and re-organizing coded and compiling, I think ultimately figuring out how to merge depends entirely on my clean disconnecting code (as well as connection failed detected code). clean disconnects should wait for all the messages to flush (that's why it's a 2-part stopping mechanism) and not accept any new requests, disconnect fails should........ ??????? do nothing except be made note of so we don't try to send the messages again? instead of sending to a known failed, we simply queue the [action-response? (broadcasts should know to try ANOTHER/known-working connection)]. Deciding on the two timeouts will be difficult. Timeout 1 = time until clean disconnect stops waiting for message responses (they might be stuck in business????), Timeout 2 = time to hold onto queued/pending/whatever-you-call-em messages because an unclean disconnect was detected
        //TODOreq: There are potentially two places our unclean disconnect can be detected. The first is when trying to send (abstract socket errors, which ARE ALSO TRIGGERED (so 3 places? 2 of which are caught in the same place?) just whenever noticed by underlying system, not ONLY when sending), the second is when the same cookie as one already in use is received.
    }
    //else: dgaf, continue hello'ing as usual. maybe we deleted their queue already? their cookie doesn't mean shit to us, but we'll still use it... fuck it
}
void MultiServerAbstraction::connectionDoneHelloing(AbstractClientConnection *abstractClientConnection)
{
    m_ListOfHelloedConnections.append(abstractClientConnection); //TODOreq: this list vs. m_ClientConnections!?!?!?!?!?!?!?!?!?!?!
}
#if 0
void MultiServerAbstraction::sendMessage(QByteArray *message, quint32 clientId)
{
    QIODevice *clientIODevice = m_ClientsByCookie.value(clientId, 0);
    if(clientIODevice)
    {
        QDataStream networkStream(clientIODevice);
        NetworkMagic::streamOutMagic(networkStream);
        networkStream << *message;
    }
    else
    {
        //TODOreq: stale or something idfk, queue?
    }
}
#endif
void MultiServerAbstraction::initialize(MultiServerAbstractionArgs multiServerAbstractionArgs)
{
    if(multiServerAbstractionArgs.m_SslTcpEnabled)
    {
        m_BeSslServer = true;
        m_SslTcpServer = new SslTcpServer(this);
        connect(m_SslTcpServer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleNewSslClientConnected(QSslSocket*)));
        m_SslTcpServer->initialize(multiServerAbstractionArgs.m_SslTcpServerArgs);
    }
}
void MultiServerAbstraction::start()
{
    if(m_BeSslServer)
    {
        m_SslTcpServer->start();
    }
    if(m_BeTcpServer)
    {
        //TODOreq
    }
    if(m_BeLocalServer)
    {
        //TODOreq
    }
}
void MultiServerAbstraction::stop()
{
    if(m_BeSslServer)
    {
        m_SslTcpServer->stop();
    }
    if(m_BeTcpServer)
    {
        //TODOreq
    }
    if(m_BeLocalServer)
    {
        //TODOreq
    }
}
void MultiServerAbstraction::handleNewClientConnected(QIODevice *newClient)
{
    emit d("new client connected, starting hello phase");

    AbstractClientConnection *newClientConnection = new AbstractClientConnection(newClient, this);
    m_ClientConnections.append(newClientConnection); //TODOreq: we need to figure out it's cookie or assign him one. if he gives us a previous one, 'merge' the two connections in this list. What does that involve? Flushing a queue? Doesn't flushing a queue mean finding out (from new connection) where exactly we are? There will be packets that will be IMPLICITLY ack'd at this phase, when we flush the queue from a given point. We may not have received a formal lazy ack ack for the ones that ARE NOT flushed in the queue (the recent coordination told us we didn't need them), but they are now implicitly acked. The reason for that is the disconnection we're recovering from may have lost us the ack ack... but the client might have sent it before knowing the connection sucked. No need to re-send them, just say where we're at and that's good enough. TODOreq _FUCK_ doesn't flushing a queue like that imply a sort of synchroncity? I thought this was supposed to be asynchronous. I'm so confused..... or maybe the lazy ack ack arrives with the retried message... so there is a 1:1 anyways and I don't need to flush any queues because he'll re-send his action requests (TODOreq: broadcasts still need to be sent/synchronized/flushed somehow)

    connect(newClientConnection, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void MultiServerAbstraction::handleNewSslClientConnected(QSslSocket *newSslClient)
{
    handleNewClientConnected(newSslClient);
}
