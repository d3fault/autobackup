#include "multiserverabstraction.h"


MultiServerAbstraction::MultiServerAbstraction(QObject *parent)
    : QObject(parent), m_SslTcpServer(0)
{
    deletePointersAndSetEachFalse();
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
    if(m_BeLocalSocketServer)
    {
        //TODOreq
    }
}
void MultiServerAbstraction::handleNewClientConnected(QIODevice *newClient)
{
    m_ServerHelloStatusesByIODevice.insert(newClient, new ServerHelloStatus());
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleNewClientSentData()));
}
void MultiServerAbstraction::handleNewClientSentData()
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
void MultiServerAbstraction::initialize(MultiServerAbstractionArgs multiServerAbstractionArgs)
{
    if(multiServerAbstractionArgs.m_SslEnabled)
    {
        m_SslTcpServer = new SslTcpServer(this);
        connect(m_SslTcpServer, SIGNAL(d(const QString &), this, SIGNAL(d(const QString &))));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleNewClientConnected(QIODevice*)));
        m_SslTcpServer->initialize(multiServerAbstractionArgs.m_SslTcpServerArgs);
    }
}
