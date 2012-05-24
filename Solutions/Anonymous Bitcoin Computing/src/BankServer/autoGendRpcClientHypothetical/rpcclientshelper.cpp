#include "rpcclientshelper.h"

RpcClientsHelper::RpcClientsHelper(IBank *rpcServerInterfaceImplementor)
    : m_RpcServerInterfaceImplementor(rpcServerInterfaceImplementor)
{
    m_ServerAndProtocolKnower = new ServerAndProtocolKnower();
    m_ServerThread = new QThread();
    m_ServerAndProtocolKnower->moveToThread(m_ServerThread);
    m_ServerThread->start();

    //NetworkAndOrProtocolReaderAndWriter *m_ServerAndProtocolKnower; //we are just abstracting the concept of the network. the object we are connecting to might be on it's own thread, and the actual QSslSocket might be on another (i don't think reading the protocol takes THAT long... but idk)

    //connect the server's signals to the IBank's slots
    connect(m_ServerAndProtocolKnower, SIGNAL(createBankAccount(const QString &)), rpcServerInterfaceImplementor, SLOT(createBankAccount(const QString &)));
    connect(rpcServerInterfaceImplementor, SIGNAL(bankAccountCreated(const QString &)), m_ServerAndProtocolKnower, SLOT(bankAccountCreated(const QString &)));

    //forward debug messages
    connect(m_ServerAndProtocolKnower, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
