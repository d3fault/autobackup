#include "rpcbankservertest.h"

RpcBankServerTest::RpcBankServerTest(QObject *parent)
    : QObject(parent), m_RpcBankServerInstantiated(false), m_RpcBankServerClientsHelperInstantiated(false), m_RpcBankServerInitialized(false), m_RpcBankServerClientsHelperInitialized(false), m_RpcBankServerClientsHelper(0)
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
    emit initializeRpcBankServerRequested(m_RpcBankServerClientsHelper);


    MultiServerAbstractionArgs serverArgs;
    serverArgs.m_SslEnabled = true;
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
void RpcBankServerTest::start()
{
    emit startRequested();
}
void RpcBankServerTest::stop()
{
    emit stopRequested();
}
void RpcBankServerTest::handleRpcBankServerInstantiated()
{
    m_RpcBankServerInstantiated = true;
    RpcBankServer *rpcBankServer = m_RpcBankServerThreadHelper.getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(initializeRpcBankServerRequested(RpcBankServerClientsHelper*)), rpcBankServer, SLOT(initialize(RpcBankServerClientsHelper*)));
    connect(rpcBankServer, SIGNAL(initialized()), this, SLOT(handleRpcBankServerInitialized()));
    connect(rpcBankServer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));

    //DEBUG:
    connect(&m_BankServerDebugWidget, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()), rpcBankServer, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()));
    connect(&m_BankServerDebugWidget, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()), rpcBankServer, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()));

    initializeBusinessAndClientsHelperIfReady();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperIntantiated()
{
    m_RpcBankServerClientsHelper = m_RpcBankServerClientsHelperThreadHelper.getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(initializeRpcBankServerClientsHelperRequested(MultiServerAbstractionArgs)), m_RpcBankServerClientsHelper, SLOT(initialize(MultiServerAbstractionArgs)));
    connect(m_RpcBankServerClientsHelper, SIGNAL(initialized()), this, SLOT(handleRpcBankServerClientsHelperInitialized()));
    connect(m_RpcBankServerClientsHelper, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    m_RpcBankServerClientsHelperInstantiated = true;
    initializeBusinessAndClientsHelperIfReady();
}
void RpcBankServerTest::handleRpcBankServerClientsHelperInitialized()
{
    m_RpcBankServerClientsHelperInitialized = true;
    emitInitializedIfBusinessAndClientsHelperAreInitialized();
}
void RpcBankServerTest::handleRpcBankServerInitialized()
{
    m_RpcBankServerInitialized = true;
    emitInitializedIfBusinessAndClientsHelperAreInitialized();
}
