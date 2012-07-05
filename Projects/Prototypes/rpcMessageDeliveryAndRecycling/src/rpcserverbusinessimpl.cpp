#include "rpcserverbusinessimpl.h"

#include "messages/actions/createbankaccountmessage.h"

void RpcServerBusinessImpl::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
    //do some shit in the db

    //if it works, just deliver. TODOreq: failed reasons cases
    emit d("business impl got create bank account request and processed it");
    createBankAccountMessage->myDeliver();
}
void RpcServerBusinessImpl::init()
{
    m_Bitcoin = new BitcoinHelperAndBroadcastMessageDispenserUser(this->broadcastDispensers());
    m_BitcoinThread = new QThread();
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();

    //meh, this is ugly but at least we know the thread isn't busy doing something else. TODOreq: make sure 2 QObjects can share a single thread. pretty sure they can but dunno. might have to make the broadcast dispensers children of m_Bitcoin instead? unsure tbh... TODOreq means research and/or test + confirm it!
    m_Bitcoin->moveBroadcastDispensersToThread(m_BitcoinThread);

    //^^^^^^^^^^^^the 'Warning' in moveToThread docs makes me pretty sure that this won't work. i think i might have to send the QThread* as a queued slot parameter to IRpcClientsHelper and then have him 'push' the dispenser(s) to the thread. as we are doing it now, i am pretty sure that they are being 'pulled'.
    //however, we ARE in the main thread at this point... so i just need to ensure that the dispensers are created in the main thread too (despite being in a different object)

    //^^^^^^^^^^^^^weirdly, the broadcasts are initialized/new'd on the main thread... but then the object that they are created in (in the constructor) is later moved to another thread (before the above is called). i'm not sure what this means. we might need to 'push' them before moving the object to it's own thread... but maybe not. i don't fully understand moveToThread. it says it moves 'children'... but i don't know if this means members or things that are initialized with QObject *parent using 'this' as the parent etc. no fucking clue tbh, need to test

    connect(m_Bitcoin, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QMetaObject::invokeMethod(m_Bitcoin, "startDebugTimer", Qt::QueuedConnection); //normally would have this schedule like once every 10 seconds max or whatever that formula was .... and it'd be checking for pending/confirmed payments etc. we are in this demo just generating a random number 1-10 and seeing if it's 3. lol
}
