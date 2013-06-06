#include "rpcbankserverclienttest.h"

RpcBankServerClientTest::RpcBankServerClientTest(QObject *parent)
    : QObject(parent), m_RpcBankServerClientInstantiated(false), m_RpcBankServerHelperInstantiated(false), m_RpcBankServerHelperInitialized(false), m_BankServerClient(0), m_RpcBankServerHelperThreadHelper(0)
{
    /*m_BankServerClient = new BankServerClient();
    m_RpcBankServerHelper = new RpcBankServerHelper(m_BankServerClient);*/

    connect(&m_RpcBankServerClientThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerClientInstantiated()));
    connect(&m_RpcBankServerHelperThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerHelperInstantiated()));

    connect(this, SIGNAL(initialized()), &m_BankServerClientDebugWidget, SLOT(businessInitialized()));
    connect(this, SIGNAL(d(QString)), &m_BankServerClientDebugWidget, SLOT(handleD(QString)));

    m_RpcBankServerClientThreadHelper.start();
    m_RpcBankServerHelperThreadHelper.start();
}
void RpcBankServerClientTest::initializeBankServerClientAndServerHelper()
{
    daisyChainConnections();

    //if there are multiple rpc services that the client needs to also connect to and get dispensers from, they are passed in as arguments here. client DEPENDS on whatever rpc shit he uses, so it makes sense after all to put that in an initialize slot. omg wtf rofl.
    emit initializeRpcBankServerClientRequested(m_RpcBankServerHelper); //TODOreq: the ServerClient both connects it's SIGNALS (which just happen to contain GUI for debugging ultimately connected) to the m_RpcBankServerHelper's Action SLOTS, and addtitionally connects to m_RpcBankServerHelper's Broadcast SIGNALS (which just happen to be connect to GUI slots for debugging)

    MultiServerClientAbstractionArgs serverClientArgs;
    serverClientArgs.m_SslTcpEnabled = true;
    serverClientArgs.m_SslTcpServerArgs.Port = 6969;
    serverClientArgs.m_SslTcpServerArgs.UseServerCA2WaySecurity = true; //this might be already implied for client, so maybe we can take it out i forget
    serverClientArgs.m_SslTcpServerArgs.ServerClientCaFilename = ":/RpcBankServerClientCA.pem";
    serverClientArgs.m_SslTcpServerArgs.ServerCaFilename = ":/RpcBankServerCA.pem";
    serverClientArgs.m_SslTcpServerArgs.ServerClientPrivateEncryptionKeyFilename = ":/RpcBankServerClientPrivateKey.pem";
    serverClientArgs.m_SslTcpServerArgs.ServerClientPublicLocalCertificateFilename = ":/RpcBankServerClientPublicCert.pem";
    serverClientArgs.m_SslTcpServerArgs.ServerClientPrivateEncryptionKeyPassPhrase = "fuckyou";

    emit initializeRpcBankServerHelperRequested(serverClientArgs);
}
void RpcBankServerClientTest::daisyChainConnections()
{
    connect(m_RpcBankServerHelper, SIGNAL(started()), m_BankServerClient, SLOT(start()));
    connect(m_BankServerClient, SIGNAL(beginStoppingProcedureRequested()), m_RpcBankServerHelper, SLOT(stop()));
    connect(m_RpcBankServerHelper, SIGNAL(stopped()), m_BankServerClient, SLOT(finishStoppingProcedure()));

    //i figured out why i'm so confus. i want to be future proof'd for "two" rpc services, yet i'm coding and habitually minimizing for the fact that i only have "one" rpc service.
    //should it be:
    //connect(m_RpcBankServerHelper, SIGNAL(stopped()), m_BankServerClient, SLOT(handleRpcBankServerHelperStopped()));
    //or
    //connect(m_RpcBankServerHelper, SIGNAL(stopped()), m_BankServerClient, SLOT(finishStoppingProcedure()));
    //both are technically correct, and i've flip flopped on the fucking issue like a jillion times and am getting completely distracted by it... distracted away from the bulk of the problem (server->client inverse merge + finish client ack)
    //ok so now right this very fucking moment decide one way of coding and stick with it. it's further complicated by the fact that "test" is the one organizing the connections between the two. BECAUSE "test" is involved (and is "god" in a design senes), it actually does seem quite alright( had:already) to use the second one (connect directly to finishStoppingProcedure). BUT DOES IT EVEN MAKE A FUCKING DIFFERENCE? NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO. This is the exact opposite of KISS. You are a fucking moron sometimes. Your intelligence slows you down sometimes ("hurrrr i could optimize dis and/or dat hurrr").

    //Code for multiple rpc services, optimize this case for the fact that it has one rpc service, merge with server (invert), custom inverting logic for source/sink nonsense (double invert!?), NEW CODE ON TOP IF IT ALL FML ROFL

    //still haven't decided how i want to KISS. DO IT MAN RIGHT THIS VERY FUCKING MOMENT.
    //Test is god
    //there decided. That's how I did it on the server so that's how I'm going to do it here.
    //LATER ON WHEN TRYING TO DO MULTIPLE RPC SERVICES SIMULTANEOUSLY IS WHEN YOU'LL NEED TO MODIFY TEST/ETC/SOMETHING with bullshit booleans and other random easy'ness (that just happens to be the feather that broke the jackass's back(brain) in terms of complexity right this very fucking moment).
    //GO. SO EASY. GO!!!!!!!!!!!
}
void RpcBankServerClientTest::handleRpcBankServerClientInstantiated()
{
    m_BankServerClient = m_RpcBankServerClientThreadHelper.getObjectPointerForConnectionsOnly();

    connect(m_BankServerClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //perform as many connections as we can right now, but the ones where bankServerClient and bankServerHelper are being connected to each other need to go in daisyChainConnections
    connect(this, SIGNAL(initializeRpcBankServerClientRequested(RpcBankServerHelper*)), m_BankServerClient, SLOT(initialize(RpcBankServerHelper*))); //I am only able to do the connection, I do not yet have the object that I'll be sending
    connect(m_BankServerClient, SIGNAL(initialized()), this, SLOT(handleRpcBankServerClientInitialized()));
    //connect(&m_BankServerClientDebugWidget, SIGNAL(startBusinessRequested()), m_BankServerClient, SLOT(startRpcBankServerHelper()));
    connect(m_BankServerClient, SIGNAL(started()), &m_BankServerClientDebugWidget, SLOT(handleBusinessStarted()));
    connect(&m_BankServerClientDebugWidget, SIGNAL(stopBusinessRequested()), m_BankServerClient, SLOT(beginStoppingProcedure()));
    connect(m_BankServerClient, SIGNAL(stopped()), &m_BankServerClientDebugWidget, SLOT(handleBusinessStopped()));

    //DEBUG
    connect(&m_BankServerClientDebugWidget, SIGNAL(simulateCreateBankAccountActionRequested(QString)), m_BankServerClient, SLOT(simulateCreateBankAccountAction(QString)));
    connect(&m_BankServerClientDebugWidget, SIGNAL(simulateGetAddFundsKeyActionRequested(QString)), m_BankServerClient, SLOT(simulateGetAddFundsKeyAction(QString)));

    m_RpcBankServerClientInstantiated = true;
    initializeBusinessAndServerHelperIfReady();
}
void RpcBankServerClientTest::handleRpcBankServerHelperInstantiated()
{
    m_RpcBankServerHelper = m_RpcBankServerHelperThreadHelper.getObjectPointerForConnectionsOnly();

    connect(m_RpcBankServerHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    connect(this, SIGNAL(initializeRpcBankServerHelperRequested(MultiServerClientAbstractionArgs)), m_RpcBankServerHelper, SLOT(initialize(MultiServerClientAbstractionArgs)));
    connect(m_RpcBankServerHelper, SIGNAL(initialized()), this, SLOT(handleRpcBankServerHelperInitialized()));
    connect(&m_BankServerClientDebugWidget, SIGNAL(startBusinessRequested()), m_RpcBankServerHelper, SLOT(start()));

    m_RpcBankServerHelperInstantiated = true;
    initializeBusinessAndServerHelperIfReady();
}
void RpcBankServerClientTest::handleRpcBankServerClientInitialized()
{
    m_RpcBankServerClientInitialized = true;
    emitInitializedIfBusinessAndServerHelperAreInitialized();
}
void RpcBankServerClientTest::handleRpcBankServerHelperInitialized()
{
    m_RpcBankServerHelperInitialized = true;
    emitInitializedIfBusinessAndServerHelperAreInitialized();
}
