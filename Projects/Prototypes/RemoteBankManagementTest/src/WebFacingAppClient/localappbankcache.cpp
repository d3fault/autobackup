#include "localappbankcache.h"

LocalAppBankCache::LocalAppBankCache(QObject *parent) :
    QObject(parent), m_BankServer(0)
{
}
void LocalAppBankCache::addUser(QString userNameToAdd)
{
    m_BankServer->addUser(userNameToAdd); //it makes sure it's connected (or waits until it is) for us
}
void LocalAppBankCache::handleUserAdded(QString newUser)
{
    //remote has added the user and signal'd us, so now we add it to our local cache and then signal the gui
    m_Db->addUser(newUser);

    emit userAdded(newUser);
}
void LocalAppBankCache::init()
{
    if(!m_BankServer)
    {
        m_BankServer = new RemoteBankServerHelper(); //starts async connecting
        connect(m_BankServer, SIGNAL(userAdded(QString)), this, SLOT(handleUserAdded(QString)));
        connect(m_BankServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    }
}
