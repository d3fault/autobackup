#include "bankserver.h"

BankServer::BankServer(QObject *parent) :
    QObject(parent), m_Bank(0)
{
}
void BankServer::startListening()
{
    if(!m_Bank)
    {
        m_Bank = new Bank();
        m_BankThread = new QThread();
        m_Bank->moveToThread(m_BankThread);
        m_BankThread->start();

        m_RpcClientsHelper = new RpcClientsHelper(m_Bank); //where input is just an IBank
        //m_RpcClientsHelper creates a thread in it's constructor for the network to use... and connects signals from the object on that thread to m_Bank, which it assumes is already on another thread. RpcClientsHelper could be on it's own thread and own the network protocol knower -- that would have the same effect... but there's not really a reason to do this. RpcClients helper just connects and disconnects signals really...

        /*
        m_ClientsThread = new QThread();
        m_Clients = new ClientsHelper();
        m_Clients->moveToThread(m_ClientsThread);
        m_ClientsThread->start();

        //connect signals and slots
        connect(m_Clients, SIGNAL(bankAccountCreationRequested(QString)), m_Bank, SLOT(handleBankAccountCreationRequested(QString)));
        connect(m_Clients, SIGNAL(balanceTransferRequested(QString,double)), m_Bank, SLOT(handleBalanceTransferRequested(QString,double)));*/

        //connect debug signals
        connect(m_Bank, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        connect(m_RpcClientsHelper, SIGNAL(d(QString)), this, SLOT(handleD(QString)));

        //daisy-chain: init Bank -> init RpcClientsHelper
        connect(m_Bank, SIGNAL(initialized()), m_RpcClientsHelper, SLOT(init()));

        //start initializing Bank
        QMetaObject::invokeMethod(m_Bank, "init", Qt::QueuedConnection);
    }
}
void BankServer::handleD(const QString &msg)
{
    qDebug() << msg;
}
