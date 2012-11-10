#include "multiserverandhelloer.h"


LEFT OFF --- didn't encounter errors but decided to singularly prototype this hello'er shit



MultiServerAndHelloer::MultiServerAndHelloer(QObject *parent, bool sslServer, bool tcpServer, bool localSocketServer)
    : m_BeSslServer(sslServer), m_BeTcpServer(tcpServer), m_BeLocalSocketServer(localSocketServer), m_BeSslServer(0)
{
}
void MultiServerAndHelloer::initSslServer()
{
    if(!m_BeSslServer)
    {
        m_SslTcpServer = new SslTcpServer(this, ":/RpcBankServerCA.pem", ":/RpcBankServerClientCA.pem", ":/RpcBankServerPrivateKey.pem", ":/RpcBankServerPublicCert.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleNewClientConnected(QIODevice*)));

        if(m_SslTcpServer->init())
        {
            emit d("SslTcpServer successfully initialized");
            emit initialized(); //TODOreq: combine signal for each server
        }
    }
}
void MultiServerAndHelloer::initTcpServer()
{
}
void MultiServerAndHelloer::initLocalSocketServer()
{
}
void MultiServerAndHelloer::startSslServer()
{
    if(m_SslTcpServer->start())
    {
        emit d("SslTcpServer successfully started");
        emit started(); //TODOreq: ditto
    }
}
void MultiServerAndHelloer::initialize()
{
    if(m_BeSslServer)
    {
        initSslServer();
    }
    if(m_BeTcpServer)
    {
        initTcpServer();
    }
    if(m_BeLocalSocketServer)
    {
        initLocalSocketServer();
    }
}
void MultiServerAndHelloer::startListening()
{
    if(m_BeSslServer)
    {
        startSslServer();
    }
    if(m_BeTcpServer)
    {

    }
    if(m_BeLocalSocketServer)
    {

    }
}
void MultiServerAndHelloer::handleNewClientConnected(QIODevice *newClient)
{
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus());
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));
}
void MultiServerAndHelloer::handleNewClientSentData()
{
    QIODevice *newClient = static_cast<QIODevice*>(sender());
    if(newClient)
    {
        ServerHelloStatus *serverHelloStatus = m_ServerHelloStatusesByIODevice.value(newClient, 0);
        if(serverHelloStatus)
        {
            switch(serverHelloStatus->serverHelloState())
            {
                case ServerHelloStatus::AwaitingHello:
                {
                    //read hello
                }
                break;
                case ServerHelloStatus::HelloReceivedWelcomeDispatchedAwaitingThankYouForWelcomingMe:
                {
                    //read thank you for welcoming me
                }
                break;
                case ServerHelloStatus::ThankYouForWelcomingMeReceivedOkStartSendingBroDispatched: //because we shouldn't get here after we're done with the hello phase. this would be an error that we forgot to disconnect or whatever...
                case ServerHelloStatus::HelloFailed:
                default:
                {
                    //TODOreq: handle errors n shit
                }
                break;
            }
        }
        else
        {
            //somehow received data from a client who skipped the handleNewClientConnected phase, wtf? should never happen. mb block the sender or fuck i have no idea what to do
        }
    }
    else
    {
        //invalid sender... should also never happen
    }
}
