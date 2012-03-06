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
        connect(&m_Db, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //requests from client
        connect(m_Clients, SIGNAL(addUserRequested(QString,QString)), this, SLOT(handleAddUserRequested(QString,QString)));
        connect(m_Clients, SIGNAL(addFundsKeyRequested(QString,QString)), this, SLOT(handleAddFundsKeyRequested(QString,QString)));

        //responding to requests from client
        connect(this, SIGNAL(userAdded(QString,QString)), m_Clients, SLOT(handleUserAdded(QString,QString)));
        connect(this, SIGNAL(addFundsKeyGenerated(QString,QString,QString)), m_Clients, SLOT(handleAddFundsKeyGenerated(QString,QString,QString)));

        QMetaObject::invokeMethod(m_Clients, "startListening", Qt::QueuedConnection);
    }
}
void Bank::handleAddUserRequested(const QString &appId, const QString &userName)
{
    //TODO: we should emit an AddUserResult or something, which deals with success, fails, and any other scenarios... and also stores the appId + userName in it (struct?)
    //i seem to be wanting this AddUserResult to be propogating from the db all the way through this server app and even back to the remote connection!! it would be nice if i can figure out way to do such a thing using just 1 type. it just might be possible. i want to avoid the mess of BankDbAddUserResult -> BankAddUserResult -> BankServerAddUserResult --[network]--> parse/process BankServerAddUserResult
    //perhaps the answer is to use an AddUserResult type struct thing (enum maybe?) in my sharedProtocol.h file

    m_Db.addUser(appId, userName);
    emit userAdded(appId, userName);
}
void Bank::handleAddFundsKeyRequested(const QString &appId, const QString &userName)
{
    //todo: should we also check here that we're not awaiting/pending... or is checking on the client/cache enough? we shouldn't even get here unless it was already checked (by the client)... but for future proofing for apps maybe it'd be good to check here (only? too?)

    QString newKey = m_Bitcoin.getNewKeyToReceivePaymentsAt();
    m_Db.setAddFundsKey(appId, userName, newKey);
    //todo: set up the code that polls this new key every so often to see if it has changed. i guess for now it's ok to iterate over m_Db and check/poll the keys for anything that's Awaiting or Pending... but in the future we'll want to have the list be separate (probably? could be wrong) and also NOT check awaitings that are > 24 hours old (or something). the user can still manually request the poll... but we definitely need to STOP polling after a certain period of time... else we may end up with hundreds of thousands of old as fuck keys that we are polling

    //we should set a timer that begins polling the list of addresses every 10 seconds. but the timer should stop while processing the list. and each item in the list should be sent as an event to ourselves so we're still able to receive addFundsKeyRequests at the same time that we're polling for older ones
    //so basically there should be a check for isLastKeyToPoll at the end of pollOneAddFundsKey (which we invokeObject queue'd).. if it evaluates to false, we launch another pollOneAddFundsKey (as an event). and lastly, if isLastKeyToPoll evalutates to true, we re-enable the 10-second timer. we don't want race conditions... which is why we disable the timer right when we start processing the list

    emit addFundsKeyGenerated(appId, userName, newKey);
}
