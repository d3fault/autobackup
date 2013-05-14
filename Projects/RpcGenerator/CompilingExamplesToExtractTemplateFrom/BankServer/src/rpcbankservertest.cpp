#include "rpcbankservertest.h"

RpcBankServerTest::RpcBankServerTest(QObject *parent)
    : QObject(parent), m_RpcBankServerInstantiated(false), m_RpcBankServerClientsHelperInstantiated(false), m_RpcBankServerInitialized(false), m_RpcBankServerClientsHelperInitialized(false), m_RpcBankServer(0), m_RpcBankServerClientsHelper(0)
{
    /*m_BankServer = new BankServer();
    m_RpcBankServerClientsHelper = new RpcBankServerClientsHelper(m_BankServer);*/
    connect(&m_RpcBankServerThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerInstantiated()));
    connect(&m_RpcBankServerClientsHelperThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerClientsHelperIntantiated()));

    connect(this, SIGNAL(initialized()), &m_BankServerDebugWidget, SLOT(businessInitialized()));
    connect(this, SIGNAL(d(QString)), &m_BankServerDebugWidget, SLOT(handleD(QString)));
    m_BankServerDebugWidget.show();

    m_RpcBankServerThreadHelper.start();
    m_RpcBankServerClientsHelperThreadHelper.start();

#if 0
    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_BankServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
#endif
}

void RpcBankServerTest::initializeBankServerAndClientsHelper()
{
    //now that we know they're both instantiated, we can set up the connections between them
    daisyChainConnections();

    emit initializeRpcBankServerRequested(m_RpcBankServerClientsHelper);


    MultiServerAbstractionArgs serverArgs;
    serverArgs.m_SslTcpEnabled = true;
    serverArgs.m_SslTcpServerArgs.HostAddress = QHostAddress::LocalHost;
    serverArgs.m_SslTcpServerArgs.Port = 6969;
    serverArgs.m_SslTcpServerArgs.UseClientCA2WaySecurity = true;
    serverArgs.m_SslTcpServerArgs.ServerCaFilename = ":/RpcBankServerCA.pem";
    serverArgs.m_SslTcpServerArgs.ClientCaFilename = ":/RpcBankServerClientCA.pem";
    serverArgs.m_SslTcpServerArgs.ServerPrivateEncryptionKeyFilename = ":/RpcBankServerPrivateKey.pem";
    serverArgs.m_SslTcpServerArgs.ServerPublicLocalCertificateFilename = ":/RpcBankServerPublicCert.pem";
    serverArgs.m_SslTcpServerArgs.ServerPrivateEncryptionKeyPassPhrase = "fuckyou"; /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */
    emit initializeRpcBankServerClientsHelperRequested(serverArgs);
}
void RpcBankServerTest::daisyChainConnections()
{
    //daisy-chaining (lazy) vs. AND'ing status signals... gah fml, both suck
    //AND'ing is async, daisy-chaining is sync. AND'ing requires sync 'flags', but could still be faster at runtime! daisy-chaining does not require async 'flags', but could be slower at runtime as each 'init' or 'start' or whatever waits for the previous
    //TODOreq: rack your brain to see if there's some kind of "catch all" solution to this, perhaps with designEquals factoring in but IDFK. Worth a good think though...
    //^^^^^^^^^A state machine that "waits" on each of the other threads to become instantiated. The longest we will wait is the length of the slowest object being instantiated on the other thread. This assumes that the previous state started them all asynchronously
    ////So perhaps a State Machine Generator (omfg famous last words or new good idea?) that just generates code and templates that I fill in. Like have them all be assert(false) stubs etc
    //////Android sort of has this already through their Activity life-span virtual overrides stuff, but mine is just to make coding on threads easier and I think my design will be vastly simpler than Android's Activity life-span bullshit setup. I never knew what was what with that... but this threading-only one won't have that same problem.
    ////////So the stop state or whatever would also wait (after async quit'ing each one first (unless order matters)), but that's built into QThread so is easy, which made me think of "what to do if it doesn't respond" which led me to think it could be solved with Byzantine shit which gave me the idea and showed me the connection/similarity between a thread/process lifespan and a distributed database "service". Both of them are solved using byzantine. You could also use the State Machine Generator instead/also/combine-the-terms-somehow to asynchronously instantiate remote byzantine-safe either services or servers, not sure what the difference is but I think understanding that might be important. Does it have to do with loading library vs. launching app similarities?


    connect(m_RpcBankServer, SIGNAL(started()), m_RpcBankServerClientsHelper, SLOT(start()));

    connect(m_RpcBankServerClientsHelper, SIGNAL(beginningStopProcedureInitiated()), m_RpcBankServer, SLOT(stop()));
    connect(m_RpcBankServer, SIGNAL(stopped()), m_RpcBankServerClientsHelper, SLOT(stop()));
}
#if 0
void RpcBankServerTest::start()
{
    //emit startRequested(); -- could make clientshelper listen to this and async start them simultaneously, but i want the business to be fully started before the clientshelper starts
    emit startRpcBankServerRequested();
}
void RpcBankServerTest::stop()
{
    emit stopRequested();
    //^perhaps this sucks. read below

    /*
    TODOreq/optional/idfk:
    Send a signal to the clientsHelper (server) saying "about to quit. don't accept any more action requests
    Send quit signal to business, might have to use flagging
    Wait on business thread
    At this point, broadcasts can still escape (pending CURRENT/ACTIVE operations) the ClientsHelper, Action Responses can too. We are purging all the current ones and then going to quit when they are empty. Since we have to wait for them to be done, the quit signal might just set up a flag?

    After we've waited for business thread to end, we send quit signal to ClientsHelper
    It can send all it's current action responses and broadcasts, flush network buffers etc, then ends cleanly
    We wait on it, then it's done! We should probably emit a "stopped" signal from this slot... but not use that "stopRequested" signal/design/method
    However, since 'this' is on the main thread, stopping might freeze the GUI. Should "stop" be asynchronous and "quit" be synchronous?
    It can be, OR 'this' can simply be moved to a backend thread. It doesn't do much anyways, just our "test" class :-).
    But the juice of this comment is basically that I need to tell ClientsHelper to stop receiving Action Requests before quitting


    I'm now trying to process that same logic BACKWARDS in order to cleanly _start_ the rpcServer/business setup :-)


    Now that I think of it, we might need to "wait" for the 'about-to-quit' signal from ClientsHelper to be responded to (queued connection call mb? gross but works. or just a signal response :-P). The reason being is that when we send the quit message to the business, we still might not have received every Action Request ever that ClientsHelper might send (before it gets to the 'about-to-quit' message). TODOreq it's a rare race condition. Business could even be 'empty' and process that 'quit' message right when it receives it (because the queue is empty) and then get messages later which aren't processed (DATA GOES MISSING = BAD BAD BAD) because the thread isn't running. ClientsHelper might be in the middle of processing a few 'new' messages that got in the queue just before the 'about to quit' message...
    */
}
#endif
void RpcBankServerTest::handleRpcBankServerInstantiated()
{
    m_RpcBankServer = m_RpcBankServerThreadHelper.getObjectPointerForConnectionsOnly();

    connect(m_RpcBankServer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));    

    //perform as many connections as we can right now, but the ones where bankServerClient and bankServerHelper are being connected to each other need to go in daisyChainConnections
    connect(this, SIGNAL(initializeRpcBankServerRequested(RpcBankServerClientsHelper*)), m_RpcBankServer, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(m_RpcBankServer, SIGNAL(initialized()), this, SLOT(handleRpcBankServerInitialized()));
    connect(&m_BankServerDebugWidget, SIGNAL(startBusinessRequested()), m_RpcBankServer, SLOT(start()));
    connect(&m_BankServerDebugWidget, SIGNAL(stopBusinessRequested()), m_RpcBankServerClientsHelper, SLOT(beginStoppingProcedure()));

    //DEBUG:
    connect(&m_BankServerDebugWidget, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()), m_RpcBankServer, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()));
    connect(&m_BankServerDebugWidget, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()), m_RpcBankServer, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()));

    m_RpcBankServerInstantiated = true;
    initializeBusinessAndClientsHelperIfReady();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperIntantiated()
{
    m_RpcBankServerClientsHelper = m_RpcBankServerClientsHelperThreadHelper.getObjectPointerForConnectionsOnly();

    connect(m_RpcBankServerClientsHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    connect(this, SIGNAL(initializeRpcBankServerClientsHelperRequested(MultiServerAbstractionArgs)), m_RpcBankServerClientsHelper, SLOT(initialize(MultiServerAbstractionArgs)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SLOT(handleRpcBankServerClientsHelperInitialized()));

    //TODOreq (design/semantics/etc):
    //we've got a weird case where our controller (should be used for initialize/start/stop) is the business also. i'm referring to RpcBankServer itself of course. My point is this: the GUI should never connect to or communicate with the RpcBankServerClientsHelper if we want "proper design"/theory. Basically a stop should look like this:
    //GUI -> Rpc Bank Server -> Rpc Bank Server Clients Helper (who then WAITS FOR RPC BANK SERVER TO FINISH WORKING ON PENDING ACTIONS BEFORE THEN SHUTTING HIMSELF DOWN (it is worth mentioning that Rpc Bank Server is still instantiated and communicable (and even data accessible i suppose (yes it buys me something but i can't put my thumb on what))) after flushing the pending action queue (or notifying that it has failed to do so given a certain timeout)) -> Rpc Bank Server -> GUI
    //whereas right now I have it (and it'll still technically work this way, just is improper design):
    //GUI -> Rpc Bank Server Clients Helper -> Rpc Bank Server [finishes up pending Actions] -> Bank Server Clients Helper -> GUI
    //They are both KIND OF fine, except that "Rpc Bank Server" is the main business and the core of the app... rpc is just an... extension to it of sorts (even though we depend on rpc atm fml (that's easily fixable by giving the init/start/stop unique-to-rpc-service names just like i'm doing in client. ultimately an include, a member, and some custom-specific-for-rpc-service-method-that-must-exist-anyways is all that's needed to use the Rpc Generator)). It's just us allowing access to the server/data.
    //I wrote this while doing the similar code in "client" and noticing that I should not be having the GUI communicate with Server Helper but instead with Server Client (who is, btw, multi-rpc-server capable so needs specialized and rpc-service specific signals/slots for starting/stopping each service based on GUI commands (or in teh future, library calls from whatever business wants to use rpc!)
    //Basically it boils down to more "requested" signals and shit like that. just a change in design really, the order of actual instantiation/initialization stays the same!
    //^^that TODOreq (design/semantics req) became: i need to make the server also having the same requirements. it essentially already does except for the naming. there would be a namespace collision in the usercode if two rpc services were generated. i just need to use the input name for (had: of) the rpc service a little bit more. instead of "initialize" it should be "handleRpcBankServerHelperInstantiated(ptr2It)". that's in rpc bank server (or soon to also become just "bank server" soon?) btw. yes, rpc bank server also has an initialize THAT INCIDENTALLY might just be calling initialize on itself and/or it's own members! the name becomes ambiguious and i don't think i was affected (or is it effected?) by the ambiguity until more recently. i could be wrong.
    //Ok figured it out: since the test is the controller and ultimately represents the user code and shoudl be minimized in complexity, we will only have communication between the GUI and the Client/Server Business. The client/server business hides all networking logic from Test (and we use signal relay hack (optimization) as needed)
    //The businesses themselves do depend on the oh fuck i'm lost as fuck
    //perhaps i need yet another insulation layer or whatever the fuck it's called. That "fake" word.

    //TODO LEFT OFF TRYING TO FIGURE OUT IF IT'S WORTH IT AND EASY TO MAKE CLIENT/SERVER BUSINESSES EASILY ABLE TO USE MULTIPLE RPC SERVICES
    //for client it only makes sense if the app is growing
    //for server it makes sense for more apps (or additionally if the data source is growing, just like with client)
    //aren't those essentially the same things? what the fuck am i talking about?
    //gah why did doing sucha trivial task suddenly become so difficult?

    connect(m_RpcBankServerClientsHelper, SIGNAL(started()), &m_BankServerDebugWidget, SLOT(handleBusinessStarted()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(stopped()), &m_BankServerDebugWidget, SLOT(handleBusinessStopped()));

    m_RpcBankServerClientsHelperInstantiated = true;
    initializeBusinessAndClientsHelperIfReady();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperInitialized()
{
    m_RpcBankServerClientsHelperInitialized = true;
    emitInitializedIfBusinessAndClientsHelperAreInitialized();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperStarted()
{
    //TODOreq perhaps? saw this empty out of nowhere
}
void RpcBankServerTest::handleRpcBankServerInitialized()
{
    m_RpcBankServerInitialized = true;
    emitInitializedIfBusinessAndClientsHelperAreInitialized();
}
