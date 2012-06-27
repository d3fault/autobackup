#include "rpcclientshelper.h"

RpcClientsHelper::RpcClientsHelper(IBankServer *rpcServerInterfaceImplementor)
    : m_RpcServerInterfaceImplementor(rpcServerInterfaceImplementor), m_ServerAndProtocolKnower(0)
{

}
void RpcClientsHelper::init()
{
    if(!m_ServerAndProtocolKnower)
    {
        //even though RpcServerInterfaceImplementor isn't yet initialized, it is already instantiated... so we CAN connect signals to it

        m_ServerAndProtocolKnower = new ServerAndProtocolKnower();
        m_ServerThread = new QThread();
        m_ServerAndProtocolKnower->moveToThread(m_ServerThread);
        m_ServerThread->start();

        //NetworkAndOrProtocolReaderAndWriter *m_ServerAndProtocolKnower; //we are just abstracting the concept of the network. the object we are connecting to might be on it's own thread, and the actual QSslSocket might be on another (i don't think reading the protocol takes THAT long... but idk)

        //connect the server's signals to the IBank's slots
        connect(m_ServerAndProtocolKnower, SIGNAL(createBankAccount(const QString &)), m_RpcServerInterfaceImplementor, SLOT(createBankAccount(const QString &)));
        connect(m_RpcServerInterfaceImplementor, SIGNAL(createBankAccountCompleted(const QString &)), m_ServerAndProtocolKnower, SLOT(createBankAccountCompleted(CreateBankAccountRequest)));

        //connect(rpcServerInterfaceImplementor, SIGNAL(pendingAm))

        //forward debug messages
        connect(m_ServerAndProtocolKnower, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //TODOreq: pretty sure the main reason I put this here is so I can connect all the signals to myself (or maybe server protocol knower) to the messages (well, associate myself with the dispensers... who connect to me/protocol-knower upon 'new' ing a message)
        rigMessages();

        emit initialized();
    }
}
void RpcClientsHelper::rigMessages()
{
    m_RpcServerInterfaceImplementor->messageDispenser()->actions()->createBankAccountMessageDispenser()->setMessageReadyReceiver(m_ServerAndProtocolKnower); //on 'new' ing, we connect the message to the appropriate SLOT

    m_RpcServerInterfaceImplementor->messageDispenser()->broadcasts()->pendingBalanceAddedDetectedMessageDispenser()->setMessageReadyReceiver(m_ServerAndProtocolKnower); //sameshit
}
