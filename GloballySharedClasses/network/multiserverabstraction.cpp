#include "multiserverabstraction.h"

quint32 MultiServerAbstraction::overflowingClientIdsUsedAsInputForMd5er = 0;

//TODOreq: no idea where this belongs, but make sure not to delete an IProtocolKnower that still has messages that are rigged for delivery() back to it that are still pending in the business. deleting it with messages still in the business would result in the silent failure of the delivery of the messages to the (new?) IProtocolKnower. Not sure if/how/where this factors in, but it probably has something to do with merging? However I think merging means we keep the protocol knower and swap out the IO device? idfk [yet].

MultiServerAbstraction::MultiServerAbstraction(IProtocolKnowerFactory *protocolKnowerFactory, QObject *parent)
    : QObject(parent), m_SslTcpServer(0) /* etc tcp local */
{
    AbstractClientConnection::setMultiServerAbstraction(this);
    AbstractClientConnection::setProtocolKnowerFactory(protocolKnowerFactory);

    deletePointersIfNotZeroAndSetEachEnabledToFalse();
}
MultiServerAbstraction::~MultiServerAbstraction()
{
    //TODOreq: flush? Perhaps it's implied when I do the delete? idfk
    //^^flush what? ideally everything will have already cleanly disconnected when we get here... so...

    deletePointersIfNotZeroAndSetEachEnabledToFalse();
}
void MultiServerAbstraction::setupSocketSpecificDisconnectAndErrorSignaling(QIODevice *ioDeviceToClient, AbstractClientConnection *abstractClientConnection)
{
    //the order that we do these is important, because ssl socket is also a tcp socket!

    QSslSocket *sslSocket_ifThatType_orZero = qobject_cast<QSslSocket*>(ioDeviceToClient);

    if(sslSocket_ifThatType_orZero)
    {
        //Ssl socket
        setupSslSocketSpecificErrorConnections(sslSocket_ifThatType_orZero, abstractClientConnection);
    }
    else
    {
        //Not Ssl Socket, so now try tcp only

        QTcpSocket *tcpSocket_ifThatType_orZero = qobject_cast<QTcpSocket*>(ioDeviceToClient);

        if(tcpSocket_ifThatType_orZero)
        {
            //Tcp Socket
            setupQAbstractSocketSpecificErrorConnections(tcpSocket_ifThatType_orZero, abstractClientConnection);
        }
        else
        {
            //Not Tcp Socket. In the future we could check for UDP or something else (threads?), but for now our 3rd is going to be localsocket --- but I mean udp is also abstract so it might just get set up if cast-able to qabstract socket at same time as tcp

            QLocalSocket *localSocket_ifThatType_orZero = qobject_cast<QLocalSocket*>(ioDeviceToClient);

            if(localSocket_ifThatType_orZero)
            {
                //Local Socket
                setupQLocalSocketSpecificErrorConnections(localSocket_ifThatType_orZero, abstractClientConnection);
            }
            else
            {
                //Not Local Socket, so give error [for now]
                emit d("Couldn't determine what kind of socket type for setting up errors");
                abstractClientConnection->makeConnectionBad(); //fuck it why not
            }
        }
    }
}
AbstractClientConnection *MultiServerAbstraction::potentialMergeCaseAsCookieIsSupplied_returning_oldConnection_ifMerge_or_ZERO_otherwise(AbstractClientConnection *newConnectionToPotentiallyMergeWithOld)
{
    //At this point, our cookie is set with the client-supplied one, but we are not done with the hello phase (we are right int he middle of it!)... so we are not on m_ListOfHelloedConnections (which means we can check against it)

    AbstractClientConnection *oldConnectionToMergeWithMaybe = getExistingConnectionUsingCookie__OrZero(newConnectionToPotentiallyMergeWithOld->cookie());
    if(oldConnectionToMergeWithMaybe)
    {
        //disable broadcasts (we pick another connection in round robin)
        dontBroadcastTo(oldConnectionToMergeWithMaybe);

        //set merge in progress, snapshotting all message IDs in business pending at this very moment. those ones require the new connection to re-request/retry them
        oldConnectionToMergeWithMaybe->setQueueActionResponsesBecauseTheyMightBeReRequestedInNewConnection(true); //TODOreq (pretty sure done): unset it later after merge complete, but make sure you figure out whether or not to send business pending that never noticed a merge occured (e.g. they were in business pending the whole time). i think i am making the client re-request all of them, so that would maybe imply we need yet another list and/or possibly even a new retry mode? there is/could-be retry-on-same-connection and retry-knowing-its-new-connection... but do i NEED to differentiate between the two?

        return oldConnectionToMergeWithMaybe;

        //merge. fuck, this seems like it should be a lot more complicated than a toMergeWithMaybe.setIoDevice(this.getIODevice), followed by a deletion of 'this' (deleteLater methinks?). Oh and I'd have to do the slot stuff too and also delete the original iodevice being overwritten???? I wonder if it's implicitly shared and has other references? Is this a security issue letting clients easily override/merge-with each other? It ultimately depends on the ssl cert being leaked i guess, or if you are dumb and choose TCP over WAN (in which case your shit can be hacked anyways~, even if you use a more elaborate username/password scheme). cert comrpomise means machine compromise means username/password compromise means fuck doing it (username/password) to begin with? I'm getting way away from the point. How the fuck do I merge two connections? This is where I keep getting stuck. I have to factor in shit like pending messages. Note that the cookie scheme is not a security measure, just an identification measure. It abstracts the connection types, because I can't use like IP address and shit for identification
        //also not sure if the server abstraction or myself should do the merging. i know one thing: IT DOESN'T FUCKING MATTER. _CHOOOOOOSE_
        //but, before i CHOOOOOOSE, figure out the fucking design of the merging! gah! brain is exploding over and over and over. there are so many places that a connection can go wrong, and i have to think of the case that accounts for all of them? or code for each separately? i can't even think about it properly. i don't know what the fuck to do. Wouldn't it involve the contents of clientshelper seeing as that's who stores the pending messages? am i going to make clientshelper an instance-per-connection type class as an optimization for accessing the m_Pending* lists, so that each connection has it's own list (along with each message type having it's own)???

        //TODOoptional: as a security measure, we could attempt to ask the OLD iodevice with the matching cookie if it is still there and if it is ok to be overwritten/whatever. if it responds at all, we pretty much want to NOT merge. if it is even able to respond, we should not be merging to begin with? i'd probably need a hard timeout for that asking


        //maybe this will become clear to me once i have the pending shit coded and working???? hmmm, since it depends on it, that actually sounds very likely

        //TODOreq: now that I have pending shit and re-organizing coded and compiling, I think ultimately figuring out how to merge depends entirely on my clean disconnecting code (as well as connection failed detected code). clean disconnects should wait for all the messages to flush (that's why it's a 2-part stopping mechanism) and not accept any new requests, disconnect fails should........ ??????? do nothing except be made note of so we don't try to send the messages again? instead of sending to a known failed, we simply queue the [action-response? (broadcasts should know to try ANOTHER/known-working connection)]. Deciding on the two timeouts will be difficult. Timeout 1 = time until clean disconnect stops waiting for message responses (they might be stuck in business????), Timeout 2 = time to hold onto queued/pending/whatever-you-call-em messages because an unclean disconnect was detected
        //TODOreq (pretty sure both points are accounted for): There are potentially two places our unclean disconnect can be detected. The first is when trying to send (abstract socket errors, which ARE ALSO TRIGGERED (so 3 places? 2 of which are caught in the same place?) just whenever noticed by underlying system, not ONLY when sending), the second is when the same cookie as one already in use is received.
    }
    //else: dgaf, continue hello'ing as usual. maybe we deleted their queue already? their cookie doesn't mean shit to us, but we'll still use it... fuck it
    return 0;
}
void MultiServerAbstraction::connectionDoneHelloing(AbstractClientConnection *abstractClientConnection)
{
    m_ListOfHelloedConnections.append(abstractClientConnection); //TODOreq: this list vs. m_ClientConnections!?!?!?!?!?!?!?!?!?!?!
}
void MultiServerAbstraction::appendDeadConnectionThatMightBePickedUpLater(AbstractClientConnection *abstractClientConnection)
{
    m_ListOfDeadConnectionsThatMightBePickedUpLater.append(abstractClientConnection);
}
AbstractClientConnection *MultiServerAbstraction::getSuitableClientConnectionNextInRoundRobinToUseForBroadcast_OR_Zero()
{
    if(m_RoundRobinQueue.length() < 1)
    {
        //needs refill
        refillRoundRobinFromHellodConnections();
    }

    //did refill succeed? we might not have any clients so don't try if that's the case
    if(m_RoundRobinQueue.length() > 0)
    {
        return m_RoundRobinQueue.dequeue();
    }

    return 0;


    //round-robin impl goes here. i want random order and "use every one forced before resetting (aka getting new random batch)"
    //i also need to make sure to NOT send to broadcasts when a connection is either not hello'd or it is attempting to merge (down until merge completes)
}
void MultiServerAbstraction::reportConnectionDestroying(AbstractClientConnection *connectionBeingDestroyed)
{
    //atm the only time i get here is when deleting a merge. i should TODOreq make clean disconnects (which i have yet to code) utilize this too, unless inappropriate
    int numRemoved = m_ListOfConnectionsIgnoringHelloState.removeAll(connectionBeingDestroyed);
    if(numRemoved != 1)
    {
        //TODO errors n shit, this should never happen
    }
    //TO DOnereq: if there isn't a point to even keeping an m_ListOfConnectionsIgnoringHelloState since I never use it for anything, then don't have it! m_ListOfHelloedConnections seems to be enough, and i could always add it back later easily
    //TODOreq: ^^^^^the above is not true it's just that i haven't written the utilizing code. it is the timing out and periodic clearing of "stuck" hellos
    //^^clearing those stuck hellos can/will be done by doing "delete extractedFromAboveListBecauseTimedOut;", which in turn makes them call this method in their destructor
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
AbstractClientConnection *MultiServerAbstraction::handleNewClientConnected(QIODevice *newClient)
{
    emit d("new client connected, starting hello phase");

    AbstractClientConnection *newClientConnection = new AbstractClientConnection(newClient, this);
    m_ListOfConnectionsIgnoringHelloState.append(newClientConnection); //TODOreq: we need to figure out it's cookie or assign him one. if he gives us a previous one, 'merge' the two connections in this list. What does that involve? Flushing a queue? Doesn't flushing a queue mean finding out (from new connection) where exactly we are? There will be packets that will be IMPLICITLY ack'd at this phase, when we flush the queue from a given point. We may not have received a formal lazy ack ack for the ones that ARE NOT flushed in the queue (the recent coordination told us we didn't need them), but they are now implicitly acked. The reason for that is the disconnection we're recovering from may have lost us the ack ack... but the client might have sent it before knowing the connection sucked. No need to re-send them, just say where we're at and that's good enough. TODOreq _FUCK_ doesn't flushing a queue like that imply a sort of synchroncity? I thought this was supposed to be asynchronous. I'm so confused..... or maybe the lazy ack ack arrives with the retried message... so there is a 1:1 anyways and I don't need to flush any queues because he'll re-send his action requests (TODOreq: broadcasts still need to be sent/synchronized/flushed somehow)

    return newClientConnection;
}
void MultiServerAbstraction::dontBroadcastTo(AbstractClientConnection *abstractClientConnection)
{
    //TODOreq: need a socket error -> reconnect with same cookie to be able to find the old connection that socket error'd, but pretty sure we do call (or SHOULD call) dontBroadcastTo when the socket error is detected also...
    int numRemoved = m_ListOfHelloedConnections.removeAll(abstractClientConnection);

    if(numRemoved == 1)
    {
        //now remove it from the round robin queue (it might not be in it btw)
        if(m_RoundRobinQueue.contains(abstractClientConnection))
        {
            numRemoved = m_RoundRobinQueue.removeAll(abstractClientConnection);
            if(numRemoved != 1)
            {
                //we had more than 1 of the same in the queue, which means our round robin code is fucked... should never happen if round robin code (etc, because we build it from m_ListOfHelloedConnections) is clean/correct
            }
        }
    }
    else
    {
        //wtf should never happen for same reason above!
    }

    //abstractClientConnection->transmitMessage();
    //round-robin hook goes here
}
void MultiServerAbstraction::refillRoundRobinFromHellodConnections()
{
    //TODOoptimization: we could 'schedule' this slot (which means we'd have to check within this method that it isn't empty (because another slot could beat us to it and do it on-demand)) whenever a connection is used from the round-robin list and it's the last one. it is a bit slower because now we check whether it's empty on every dequeue, but at least that optimization would move it to being a background task (but not really by much because it still takes clock cycles. the main purpose of the optimization is solely to speed up the time it takes to broadcast. if there are lots of connections it might take longer than desired to send a broadcast because we have to refill the list. a ghetto round robin that just selects a random index into m_ListOfHelloedConnections would be the fastest of course, but then they wouldn't be load balanced in the sense that every connection is used once before they start over. still a bit of load balancing though...

    QList<int> indexesRemaining;

    int hellodConnectionsSize = m_ListOfHelloedConnections.length();
    for(int i = 0; i < hellodConnectionsSize; ++i)
    {
        indexesRemaining.append(i);
    }

    while(indexesRemaining.length() > 0)
    {
        int randomIndexIntoIndexList = (qrand() % indexesRemaining.length());
        int indexChosenRandomlyFromIndexList = indexesRemaining.takeAt(randomIndexIntoIndexList);
        m_RoundRobinQueue.enqueue(m_ListOfHelloedConnections.at(indexChosenRandomlyFromIndexList));
    }
}
void MultiServerAbstraction::setupQAbstractSocketSpecificErrorConnections(QAbstractSocket *abstractSocket, AbstractClientConnection *abstractClientConnection)
{
    //TODOreq: shouldn't error detections also go into "potentialMergeMode"? yes. anything except clean disconnect (or never finished hello'ing to begin with!) goes into potential merge mode for a period of time. so i need to do more than just make the connection go bad! basically i think i should have a flag in a signal listening to disconnected that checks whether it was a clean disconnect or not. the bool is only set to disconnectExpected = true whenever a clean disconnect is ready (request queue has been processed and responded to)

    connect(abstractSocket, SIGNAL(error(QAbstractSocket::SocketError)), abstractClientConnection, SLOT(makeConnectionBad()));
    connect(abstractSocket, SIGNAL(disconnected()), abstractClientConnection, SLOT(makeConnectionBad()));
    connect(abstractSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), abstractClientConnection, SLOT(makeConnectionBadIfNewSocketStateSucks(QAbstractSocket::SocketState)));
}
void MultiServerAbstraction::setupSslSocketSpecificErrorConnections(QSslSocket *sslSocket, AbstractClientConnection *abstractClientConnection)
{
    setupQAbstractSocketSpecificErrorConnections(sslSocket, abstractClientConnection); //since ssl is a layer on top of tcp

    connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), abstractClientConnection, SLOT(makeConnectionBad()));
}
void MultiServerAbstraction::setupQLocalSocketSpecificErrorConnections(QLocalSocket *localSocket, AbstractClientConnection *abstractClientConnection)
{
    //TODOreq: for abstract socket, ssl, and local socket, I am dropping the contents of the errors and just utilizing the fact that an error occured. I should later on hook them up to be debug outputted too -- for debugging purposes only obviously... whereas right now I'm hooking into them for functionality. The same applies to state changes for both local/abstract, i should output each
    connect(localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), abstractClientConnection, SLOT(makeConnectionBad()));
    connect(localSocket, SIGNAL(disconnected()), abstractClientConnection, SLOT(makeConnectionBad()));
    connect(localSocket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)), abstractClientConnection, SLOT(makeConnectionBadIfNewQLocalSocketStateSucks(QLocalSocket::LocalSocketState)));
}
void MultiServerAbstraction::handleNewSslClientConnected(QSslSocket *newSslClient)
{
    AbstractClientConnection *abstractClientConnectionSoICanConnectToSocketTypeSpecificDisconnectSignals = handleNewClientConnected(newSslClient);

    setupSslSocketSpecificErrorConnections(newSslClient, abstractClientConnectionSoICanConnectToSocketTypeSpecificDisconnectSignals);
}
