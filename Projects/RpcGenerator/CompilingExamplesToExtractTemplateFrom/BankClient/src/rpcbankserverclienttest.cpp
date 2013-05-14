#include "rpcbankserverclienttest.h"

RpcBankServerClientTest::RpcBankServerClientTest()
    : QObject(parent), m_RpcBankServerClientInstantiated(false), m_RpcBankServerHelperInstantiated(false), m_RpcBankServerHelperInitialized(false), m_BankServerClient(0), m_RpcBankServerHelperThreadHelper(0)
{
    /*m_BankServerClient = new BankServerClient();
    m_RpcBankServerHelper = new RpcBankServerHelper(m_BankServerClient);*/

    connect(&m_RpcBankServerClientThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerClientInstantiated()));
    connect(&m_RpcBankServerHelperThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRpcBankServerHelperInstantiated()));

    connect(this, SIGNAL(initialized()), &m_BankServerClientDebugWidget, SLOT(businessInitialized()));
    connect(this, SIGNAL(d(QString)), &m_BankServerClientDebugWidget, SLOT(handleD(QString)));
    m_BankServerClientDebugWidget.show();

    m_RpcBankServerClientThreadHelper.start();
    m_RpcBankServerHelperThreadHelper.start();
#if 0
    connect(m_RpcBankServerHelper, SIGNAL(initialized()), this, SIGNAL(initialized()));
    connect(m_RpcBankServerHelper, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_RpcBankServerHelper, SIGNAL(stopped()), this, SIGNAL(stopped()));

    connect(m_BankServerClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_RpcBankServerHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
#endif
}
void RpcBankServerClientTest::initializeBankServerClientAndServerHelper()
{
    daisyChainConnections();

    emit rpcBankServerHelperInstantiated(m_RpcBankServerHelper); //TODOreq: the ServerClient both connects it's SIGNALS (which just happen to contain GUI for debugging ultimately connected) to the m_RpcBankServerHelper's Action SLOTS, and addtitionally connects to m_RpcBankServerHelper's Broadcast SIGNALS (which just happen to be connect to GUI slots for debugging)

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

    connect(m_BankServerClient, SIGNAL(beginningStopProcedureInitiated()), m_RpcBankServerHelper, SLOT(stop()));
    connect(m_RpcBankServerHelper, SIGNAL(stopped()), m_BankServerClient, SLOT(stop()));
}
#if 0
void RpcBankServerClientTest::init()
{
    m_RpcBankServerHelper->init();
}
void RpcBankServerClientTest::start()
{
    m_RpcBankServerHelper->start();
}
void RpcBankServerClientTest::stop()
{
    m_RpcBankServerHelper->stop();
}
void RpcBankServerClientTest::simulateCreateBankAccountAction(QString username)
{
    QMetaObject::invokeMethod(m_BankServerClient, "simulateCreateBankAccountAction", Qt::QueuedConnection, Q_ARG(QString, username));
}
void RpcBankServerClientTest::simulateGetAddFundsKeyAction(QString username)
{
    QMetaObject::invokeMethod(m_BankServerClient, "simulateGetAddFundsKeyAction", Qt::QueuedConnection, Q_ARG(QString, username));
}
#endif
void RpcBankServerClientTest::handleRpcBankServerClientInstantiated()
{
    m_BankServerClient = m_RpcBankServerClientThreadHelper.getObjectPointerForConnectionsOnly();

    connect(m_BankServerClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //perform as many connections as we can right now, but the ones where bankServerClient and bankServerHelper are being connected to each other need to go in daisyChainConnections
    connect(this, SIGNAL(rpcBankServerHelperInstantiated(RpcBankServerHelper*)), m_BankServerClient, SLOT(handleRpcBankServerHelperInstantiated(RpcBankServerHelper*))); //I am only able to do the connection, I do not yet have the object that I'll be sending
    connect(m_BankServerClient, SIGNAL(initialized()), this, SLOT(handleRpcBankServerClientInitialized()));
    connect(&m_BankServerClientDebugWidget, SIGNAL(startBusinessRequested()), m_BankServerClient, SLOT(startRpcBankServerHelper()));
    connect(m_BankServerClient, SIGNAL(started()), &m_BankServerClientDebugWidget, SLOT(handleBusinessStarted()));
    connect(&m_BankServerClientDebugWidget, SIGNAL(stopBusinessRequested()), m_BankServerClient, SLOT(beginStoppingRpcBankServerHelper()));
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

    m_RpcBankServerHelperInstantiated = true;
    initializeBusinessAndServerHelperIfReady();
}
void RpcBankServerClientTest::handleRpcBankServerClientInitialized()
{
}
void RpcBankServerClientTest::handleRpcBankServerHelperInitialized()
{
}
