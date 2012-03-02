#include "bank.h"

Bank::Bank(QObject *parent) :
    QObject(parent), m_Clients(0)
{

}
void Bank::start()
{
    if(!m_Clients)
    {
        m_ServerThread = new QThread();
        m_Clients = new AppClientHelper();
        m_Clients->moveToThread(m_ServerThread);
        m_ServerThread->start();

        connect(m_Clients, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_Clients, SIGNAL(addUserRequested(QString,QString)), this, SLOT(handleAddUserRequested(QString,QString)));
        connect(this, SIGNAL(userAdded(QString,QString)), m_Clients, SLOT(handleUserAdded(QString,QString)));

        QMetaObject::invokeMethod(m_Clients, "startListening", Qt::QueuedConnection);
    }
}
void Bank::handleAddUserRequested(const QString &appId, const QString &userName)
{
    //TODO: we should emit an AddUserResult or something, which deals with success, fails, and any other scenarios... and also stores the appId + userName in it (struct?)
    //i seem to be wanting this AddUserResult to be propogating from the db all the way through this server app and even back to the remote connection!! it would be nice if i can figure out way to do such a thing using just 1 type. it just might be possible. i want to avoid the mess of BankDbAddUserResult -> BankAddUserResult -> BankServerAddUserResult --[network]--> parse/process BankServerAddUserResult
    //perhaps the answer is to use an AddUserResult type struct thing (enum maybe?) in my sharedProtocol.h file

    m_Db.addUser(appId, userName);
    emit d("server added user: '" + userName + "'' to appId account: '" + appId + "'");
    emit userAdded(appId, userName);
}
