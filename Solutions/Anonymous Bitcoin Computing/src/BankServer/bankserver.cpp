#include "bankserver.h"

BankServer::BankServer(QObject *parent) :
    QObject(parent), m_Bank(0)
{
}
void BankServer::startListening()
{
    if(!m_Bank)
    {
        m_BankThread = new QThread();
        m_Bank = new Bank();
        m_Bank->moveToThread(m_BankThread);
        m_BankThread->start();

        m_ClientsThread = new QThread();
        m_Clients = new ClientsHelper();
        m_Clients->moveToThread(m_ClientsThread);
        m_ClientsThread->start();

        //connect signals and slots
        connect(m_Clients, SIGNAL(bankAccountCreationRequested(QString)), m_Bank, SLOT(handleBankAccountCreationRequested(QString)));
        connect(m_Clients, SIGNAL(balanceTransferRequested(QString,double)), m_Bank, SLOT(handleBalanceTransferRequested(QString,double)));

        //connect debug signals
        connect(m_Bank, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        connect(m_Clients, SIGNAL(d(QString)), this, SLOT(handleD(QString)));

        //daisy-chain: init Bank -> init ClientsHelper
        connect(m_Bank, SIGNAL(initialized()), m_Clients, SLOT(init()));

        //start initializing Bank
        QMetaObject::invokeMethod(m_Bank, "init", Qt::QueuedConnection);
    }
}
void BankServer::handleD(const QString &msg)
{
    qDebug() << msg;
}
