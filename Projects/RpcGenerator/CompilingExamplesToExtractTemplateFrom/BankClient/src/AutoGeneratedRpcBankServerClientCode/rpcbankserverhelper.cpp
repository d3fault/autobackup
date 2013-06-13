#include "rpcbankserverhelper.h"

RpcBankServerHelper::RpcBankServerHelper(QObject *parent)
    : IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack(parent), m_RpcBankServerClientProtocolKnowerFactory(this), m_MultiServerClientAbstraction(&m_RpcBankServerClientProtocolKnowerFactory, this)
{
    RpcBankServerClientProtocolKnowerFactory::setSignalRelayHackEmitter(this);
    //m_Transporter = new SslTcpClientAndBankServerProtocolKnower();

    //m_ActionDispensers = new RpcBankServerClientActionDispensers(m_Transporter);
    //m_BroadcastDispensers = new RpcBankServerClientBroadcastDispensers(m_Transporter); //we (client) don't need a destination, but since it's shared code with the server we just deal with it and make our destination forward to recycling

    m_ActionDispensers = new RpcBankServerClientActionDispensers(this);

    //m_BroadcastDispensers = new RpcBankServerClientBroadcastDispensers(this);
    //TODOreq: not sure where Broadcast Dispensers gets new'd because I'm not sure where ActionDispensers gets new'd in the server equivalent of this code...

    connect(&m_MultiServerClientAbstraction, SIGNAL(readyForActionRequests()), this, SIGNAL(readyForActionRequests()));

#if 0
    m_RpcBankServerClient->setActionDispensers(m_ActionDispensers);
    m_Transporter->setBroadcastDispensers(m_BroadcastDispensers);

    m_RpcBankServerClient->instructBackendObjectsToClaimRelevantDispensers();

    if(!m_ActionDispensers->everyDispenserIsCreated())
        return; //TODOreq fail properly, see RpcBankServerClientsHelper on server

    m_Transporter->takeOwnershipOfBroadcastsAndSetupDelivery();

    m_RpcBankServerClient->moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();

    moveTransporterToItsOwnThreadAndStartTheThread();

    moveBusinessToItsOwnThreadAndStartTheThread();

    m_RpcBankServerClient->connectRpcBankServerSignalsToBankServerClientImplSlots(m_Transporter);
    daisyChainInitStartStopConnections();

    connect(m_Transporter, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
#endif
}
void RpcBankServerHelper::sendActionRequestToArandomServer(IActionMessage *actionRequest)
{
}
#if 0
void RpcBankServerHelper::daisyChainInitStartStopConnections()
{
    //daisy-chain connections
    //init
    connect(m_RpcBankServerClient, SIGNAL(initialized()), m_Transporter, SLOT(init()));
    connect(m_Transporter, SIGNAL(initialized()), this, SIGNAL(initialized()));

    //start
    connect(m_RpcBankServerClient, SIGNAL(started()), m_Transporter, SLOT(start()));
    connect(m_Transporter, SIGNAL(started()), this, SIGNAL(started()));

    //stop
    connect(m_Transporter, SIGNAL(stopped()), m_RpcBankServerClient, SLOT(stop()));
    connect(m_RpcBankServerClient, SIGNAL(stopped()), this, SIGNAL(stopped()));
}
void RpcBankServerHelper::init()
{
    //old:
    //emit d("RpcBankServerHelper received init message");
    //QMetaObject::invokeMethod(m_RpcBankServerClient, "init", Qt::QueuedConnection);

    //TODOreq:

    //then
    emit initialized();

    //TODOreq: same redo from old->new for start/stop as well
}
void RpcBankServerHelper::moveTransporterToItsOwnThreadAndStartTheThread()
{
    m_TransporterThread = new QThread();
    m_Transporter->moveToThread(m_TransporterThread);
    m_TransporterThread->start();
}
void RpcBankServerHelper::moveBusinessToItsOwnThreadAndStartTheThread()
{
    m_BusinessThread = new QThread();
    m_RpcBankServerClient->moveToThread(m_BusinessThread);
    m_BusinessThread->start();
}
#endif
void RpcBankServerHelper::initialize(MultiServerClientsAbstractionArgs multiServerClientAbstractionArgs)
{
    m_MultiServerClientAbstractionArgs_HACK2PASS2START = multiServerClientAbstractionArgs;

    emit d("RpcBankServerHelper received initialize message");
    //TODOreq: appears to make sense that the .initailize call below should return a bool and be checked before setting m_Initialized to true...



    m_Initialized = true;
    emitInitializedSignalIfReady();
}
void RpcBankServerHelper::handleBusinessDoneClaimingActionDispensersAndConnectingToBroadcastSignals()
{
    emitInitializedSignalIfReady();
}
void RpcBankServerHelper::start()
{
    emit d("RpcBankServerHelper received start message");

    m_MultiServerClientAbstraction.initializeAndStartConnections(m_MultiServerClientAbstractionArgs_HACK2PASS2START); //TO DOnereq: can't start here because we don't know if all action dispensers are claimed yet (emitInitializedSignalIfReady)!!!! Fuck man I need to sleep on this design and meh this merge is going to take forever but at least most of it is relatively easy... TODO LEFT OFF
    //We need the broadcast dispensers to be claimed because a broadcast might come right away on a connection that's been started. Just a race condition but definitely worth fixing
    //^^Moving this to "start" is one way of making sure broadcasts are already connected to... but it's still a TODOreq I think to have "initialize" and "start" work properly. Fuck it for now
    //TODOreq: i think this the first time i realized it [recently]: We also need to make sure that the Action RESPONSE signals are connected to as well? I can use the same connect notify scheme I devised for broadcasts (and of course ignoring certain Action Request dispenser claims should handle ignoring the Action Response signal connections -- shouldn't require more calls)

    m_MultiServerClientAbstraction.start();

    emit started();
}
void RpcBankServerHelper::createBankAccountDelivery()
{
    //ehhh wat


    //aren't action requests supposed to be delivered directly to their protocol knower?
    //yes

    //BUT

    //that would mean that the business would need a dispenser for each connection...
    //so NO

    //TODOreq: i could do a round robin in the getNewOrRecycled function in order to make the action requests go directly to protocol knowers of specific connections, OR i can just leave it as a single dispenser and have it send action requests to here, who then send them to multi server who does round robin'ing etc

    //it's just weird because i thought i already coded this in protocol knower!



    //Actually starting to think maybe my design is broken. Broadcasts on the server will have the same problem and the code I have in place for them doesn't really do anything with acks (therefore doesn't do anything with protocol knower). We _COULD_ do the ack'ing right here but then it'd be less efficient.
    //Ugg I feel sick to my stomach while thinking about how fucked the design [might] be
    //TODO LEFT OFF -- I have this exact same method (already coded) in the inherited protocol knower, where it appears to belong as an optimization so we only ever have to search in lists of messages that belong to a given connection. Doing it here means we'd have to search all messages in all connections when doing operations. Starting to like that round robin in getNewOrRecycled idea, but I bet even that would be a bitch to implement.

    //I also might be able to just hackily pass it to multi server abstraction who then picks a protocol knower from round robin and then protocol knower does it's regular schtick (which is already coded) -- sort of like the signal relay hack emitter except for slots lol

    //TODO LEFT OFF -- Slept on it and pretty sure I want to just do a slot relay hack out of KISS and because re-arranging the dispenser logic to do the round robin'ing sounds too hard. Since helper, multi*abstraction, and protocol knowers, are all on teh same thread... doing it this way is fine. But that did make me think of something else:

    //TODOreq: since rpc*helper (on both sides of the network communication) rely on 'sender()' to get Action Requests (on client) and Broadcasts (on server).... AND that we are relying on the "user" to instantiate rpc*helper himself... if he were to put rpc*helper and the 'business' on the same thread, then the AutoConnection functionality of QObject::connect would make sender() not work! One solution to this is to have rpc*helper just be a high level wrapper ish that then starts a child thread. It would need to make whatever objects are on that child thread (basically ITSELF, but copied into the child object on the thread it now manages) now in charge of the "signal relay hacks" (and slot ones too since it looks like i'll be implementing that soon). It's mostly just a high level design change... and I'm not too keen on it because it's better to NOT do threads and handle them later...  whereas now we'd be forcing a thread. I can't think of a solution to this other than that. Also the rpc*helper would be called via direct calls (it would live on the GUI thread (or whoever instantiated it) and would be the one responsible for "safely communicating with the thread it creates (basically the network thread)". It would become a controller of sorts
}
void RpcBankServerHelper::getAddFundsKeyDelivery()
{
}
void RpcBankServerHelper::stop()
{
    emit d("RpcBankServerHelper received stop message");
    m_MultiServerClientAbstraction.stop();
    emit stopped();
}
