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

    connect(&m_BankServerDebugWidget, SIGNAL(startBusinessRequested()), m_RpcBankServer, SLOT(start()));
    connect(m_RpcBankServer, SIGNAL(started()), m_RpcBankServerClientsHelper, SLOT(start()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(started()), &m_BankServerDebugWidget, SLOT(handleBusinessStarted()));

    connect(&m_BankServerDebugWidget, SIGNAL(stopBusinessRequested()), m_RpcBankServerClientsHelper, SLOT(beginStoppingProcedure()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(beginningStopProcedureInitiated()), m_RpcBankServer, SLOT(stop()));
    connect(m_RpcBankServer, SIGNAL(stopped()), m_RpcBankServerClientsHelper, SLOT(stop()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(stopped()), &m_BankServerDebugWidget, SLOT(handleBusinessStopped()));
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
    connect(this, SIGNAL(initializeRpcBankServerRequested(RpcBankServerClientsHelper*)), m_RpcBankServer, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(m_RpcBankServer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
    connect(m_RpcBankServer, SIGNAL(initialized()), this, SLOT(handleRpcBankServerInitialized()));

    connect(&m_BankServerDebugWidget, SIGNAL(startBusinessRequested()), m_RpcBankServer, SLOT(start()));
    connect(&m_BankServerDebugWidget, SIGNAL(stopBusinessRequested()), m_RpcBankServer, SLOT(stop()));

    //DEBUG:
    connect(&m_BankServerDebugWidget, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()), m_RpcBankServer, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()));
    connect(&m_BankServerDebugWidget, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()), m_RpcBankServer, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()));

    m_RpcBankServerInstantiated = true;
    initializeBusinessAndClientsHelperIfReady();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperIntantiated()
{
    m_RpcBankServerClientsHelper = m_RpcBankServerClientsHelperThreadHelper.getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(initializeRpcBankServerClientsHelperRequested(MultiServerAbstractionArgs)), m_RpcBankServerClientsHelper, SLOT(initialize(MultiServerAbstractionArgs)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SLOT(handleRpcBankServerClientsHelperInitialized()));

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
}
void RpcBankServerTest::handleRpcBankServerInitialized()
{
    m_RpcBankServerInitialized = true;
    emitInitializedIfBusinessAndClientsHelperAreInitialized();
}
