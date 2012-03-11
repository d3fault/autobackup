#include "bank.h"


/*
  -testing-
  cd into bitcoin's /bin directory, then start up the two bitcoind daemon's in test mode
  ./bitcoind -datadir=/home/d3fault/test/btc/testnet-box/1 -daemon
  ./bitcoind -datadir=/home/d3fault/test/btc/testnet-box/2 -daemon

  optional: if you are testing receiving confirmed payments, enable generating of the blocks (on either or both)
  ./bitcoind -datadir=/home/d3fault/test/btc/testnet-box/1 setgenerate true
  ./bitcoind -datadir=/home/d3fault/test/btc/testnet-box/2 setgenerate true

  then in the app you do 'add funds' to send a 'getnewaddress' to bitcoind
  copy that address and 'sendtoaddress' from #1 (our app is #2):
  ./bitcoind -datadir=/home/d3fault/test/btc/testnet-box/1 sendtoaddress <key-copied> 123.456789
*/


Bank::Bank(QObject *parent) :
    QObject(parent), m_Clients(0)
{
}
void Bank::start()
{
    if(!m_Clients)
    {
        m_Db = new BankDb();
        m_BitcoinPoller = new BitcoinPoller(10);

        m_ServerThread = new QThread();
        m_Clients = new AppClientHelper();
        m_Clients->moveToThread(m_ServerThread);
        m_ServerThread->start();

        //set up debugging
        connect(m_Clients, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_Db, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(BitcoinHelper::Instance(), SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_BitcoinPoller, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

        //HANDLING REQUESTS FROM CLIENTS
        connect(m_Clients, SIGNAL(addUserRequested(QString,QString)), this, SLOT(handleAddUserRequested(QString,QString)));
        connect(m_Clients, SIGNAL(addFundsKeyRequested(QString,QString)), this, SLOT(handleAddFundsKeyRequested(QString,QString)));

        //RESPONDING TO REQUESTS FROM CLIENTS
        //user added
        connect(this, SIGNAL(userAdded(QString,QString)), m_Clients, SLOT(handleUserAdded(QString,QString)));
        //key generated
        connect(this, SIGNAL(addFundsKeyGenerated(QString,QString,QString)), m_Clients, SLOT(handleAddFundsKeyGenerated(QString,QString,QString)));
        connect(this, SIGNAL(addFundsKeyGenerated(QString,QString,QString)), m_BitcoinPoller, SLOT(pollThisKeyAndNotifyUsOfUpdates(QString,QString,QString))); //it might be better to do this as a Qt::QueuedConnection. they currently live on the same thread so it will be called direct. after i write some more code i'll be able to tell. shouldn't matter but it might

        //bitcoin poller
        //-- poller -> this
        connect(m_BitcoinPoller, SIGNAL(pendingAmountDetected(QString,QString,QString,double)), this, SLOT(handlePendingAmountDetected(QString,QString,QString,double)));
        connect(m_BitcoinPoller, SIGNAL(confirmedAmountDetected(QString,QString,QString,double)), this, SLOT(handleConfirmedAmountDetected(QString,QString,QString,double)));
        //^^in these two slots, we update the db and then emit the corresponding "received" signals that then notify the m_Clients, done below VVV
        //-- this -> clients
        connect(this, SIGNAL(pendingAmountReceived(QString,QString,QString,double)), m_Clients, SLOT(handlePendingAmountReceived(QString,QString,QString,double)));
        connect(this, SIGNAL(confirmedAmountReceived(QString,QString,QString,double)), m_Clients, SLOT(handleConfirmedAmountReceived(QString,QString,QString,double)));

        QMetaObject::invokeMethod(m_Clients, "startListening", Qt::QueuedConnection);
    }
}
void Bank::handleAddUserRequested(const QString &appId, const QString &userName)
{
    //TODO: we should emit an AddUserResult or something, which deals with success, fails, and any other scenarios... and also stores the appId + userName in it (struct?)
    //i seem to be wanting this AddUserResult to be propogating from the db all the way through this server app and even back to the remote connection!! it would be nice if i can figure out way to do such a thing using just 1 type. it just might be possible. i want to avoid the mess of BankDbAddUserResult -> BankAddUserResult -> BankServerAddUserResult --[network]--> parse/process BankServerAddUserResult
    //perhaps the answer is to use an AddUserResult type struct thing (enum maybe?) in my sharedProtocol.h file

    m_Db->addUser(appId, userName);
    emit userAdded(appId, userName);
}
void Bank::handleAddFundsKeyRequested(const QString &appId, const QString &userName)
{
    //todo: should we also check here that we're not awaiting/pending... or is checking on the client/cache enough? we shouldn't even get here unless it was already checked (by the client)... but for future proofing for apps maybe it'd be good to check here (only? too?)

    QString newKey = BitcoinHelper::Instance()->getNewKeyToReceivePaymentsAt();
    m_Db->setAddFundsKey(appId, userName, newKey);
    //todo: set up the code that polls this new key every so often to see if it has changed. i guess for now it's ok to iterate over m_Db and check/poll the keys for anything that's Awaiting or Pending... but in the future we'll want to have the list be separate (probably? could be wrong) and also NOT check awaitings that are > 24 hours old (or something). the user can still manually request the poll... but we definitely need to STOP polling after a certain period of time... else we may end up with hundreds of thousands of old as fuck keys that we are polling

    //we should set a timer that begins polling the list of addresses every 10 seconds. but the timer should stop while processing the list. and each item in the list should be sent as an event to ourselves so we're still able to receive addFundsKeyRequests at the same time that we're polling for older ones
    //so basically there should be a check for isLastKeyToPoll at the end of pollOneAddFundsKey (which we invokeObject queue'd).. if it evaluates to false, we launch another pollOneAddFundsKey (as an event). and lastly, if isLastKeyToPoll evalutates to true, we re-enable the 10-second timer. we don't want race conditions... which is why we disable the timer right when we start processing the list

    emit addFundsKeyGenerated(appId, userName, newKey); //poller can just listen to this signal. <3 PROPER design.
}
void Bank::handlePendingAmountDetected(QString appId, QString username, QString key, double pendingAmount)
{
    m_Db->pendingAmountReceived(appId, username, pendingAmount);

    emit d(username + " received pending amount: " + QString::number(pendingAmount,'f',8));

    emit pendingAmountReceived(appId, username, key, pendingAmount);
}
void Bank::handleConfirmedAmountDetected(QString appId, QString username, QString key, double confirmedAmount)
{
    m_Db->confirmedAmountReceived(appId, username, confirmedAmount);

    emit d(username + " received confirmed amount: " + QString::number(confirmedAmount, 'f', 8));

    emit confirmedAmountReceived(appId, username, key, confirmedAmount);
}

//TODO: when converting a double to a QString, the format parameter should be 'f', with precision of 8. MAYBE a precision of 9 and we manually round it??? the default format uses e and E... but we don't want that so we have to force 'f'
//when converting from QString to double, it should just work.. but we can check the &ok to make sure it's true afterwards
