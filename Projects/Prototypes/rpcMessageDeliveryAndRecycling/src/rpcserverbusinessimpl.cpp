#include "rpcserverbusinessimpl.h"

#include <QThread> //for debugging threadId

#include "messages/actions/createbankaccountmessage.h"

RpcServerBusinessImpl::RpcServerBusinessImpl(QObject *parent)
    : IRpcServerImpl(parent)
{
    //maybe it'd work better if i explicitly set m_Bitcoin as a child of 'this', by passing 'this' in as QObject *parent in constructor. wouldn't that also be a way of moving m_Bitcoin to...........
    //wait no... the other way around... m_Bitcoin passes himself into the broadcastDispenser QObject *parent constructors. i GUESS we could have the business impl instantiate the broadcast dispensers. they should NOT allow the default of 0 for the parent. but i wonder if i set moveToThread before explicitly setting them as a child.. if it still is the equivalent of doing .moveToThread
    //it might be safest to do both... but that might also lead to unpredictable errors or code paths (and could be platform dependent errors :-/). i really don't know which is better. i _DO_ know that if you call moveToThread, all of the object's 'children' are moved as well. since that's the only bit of information, it's the only information i should rely on. so perhaps i don't need this pushBroadcastDispensersToAppropriateBusinessThreads. i wish there was an easier way to see what thread i'm on. thread id or something?

    m_Bitcoin = new BitcoinHelperAndBroadcastMessageDispenserUser();
    m_BitcoinThread = new QThread();
}
void RpcServerBusinessImpl::pushBroadcastDispensersToAppropriateBusinessThreads()
{
    m_Bitcoin->grabBroadcastDispensersAndMoveEachToOurThread(m_BroadcastDispensers, m_BitcoinThread); //for the time being, the broadcastDispensers occupy that thread alone. i guess i could move the rest of 'init' up here... but it matters not... and technically initializes faster (a gui or something). premature optimi-
    //i had this in the constructor at first but it needs to be a) after OUR constructor and b) after 'this' is passed into IRpcClientsHelper... which is where the broadcastDispensers get set up.
    //oh and c), it needs to be before 'this' is moved to another thread.. and possibly d) before bitcoin is moved to it's thread (though am unsure about that, seeing as i call a public method directly it shouldn't matter)
}
void RpcServerBusinessImpl::createBankAccount(CreateBankAccountMessage *createBankAccountMessage)
{
    //do some shit in the db

    //if it works, just deliver. TODOreq: failed reasons cases
    emit d(QString("business impl got create bank account request and processed it on thread: ") + QString::number(QThread::currentThreadId()));
    createBankAccountMessage->myDeliver();
}
void RpcServerBusinessImpl::init()
{
    m_Bitcoin->moveToThread(m_BitcoinThread);
    m_BitcoinThread->start();

    //meh, this is ugly but at least we know the thread isn't busy doing something else. TODOreq: make sure 2 QObjects can share a single thread. pretty sure they can but dunno. might have to make the broadcast dispensers children of m_Bitcoin instead? unsure tbh... TODOreq means research and/or test + confirm it!

    //^^^^^^^^^^^^the 'Warning' in moveToThread docs makes me pretty sure that this won't work. i think i might have to send the QThread* as a queued slot parameter to IRpcClientsHelper and then have him 'push' the dispenser(s) to the thread. as we are doing it now, i am pretty sure that they are being 'pulled'.
    //however, we ARE in the main thread at this point... so i just need to ensure that the dispensers are created in the main thread too (despite being in a different object)

    //^^^^^^^^^^^^^weirdly, the broadcasts are initialized/new'd on the main thread... but then the object that they are created in (in the constructor) is later moved to another thread (before the above is called). i'm not sure what this means. we might need to 'push' them before moving the object to it's own thread... but maybe not. i don't fully understand moveToThread. it says it moves 'children'... but i don't know if this means members or things that are initialized with QObject *parent using 'this' as the parent etc. no fucking clue tbh, need to test

    connect(m_Bitcoin, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    QMetaObject::invokeMethod(m_Bitcoin, "startDebugTimer", Qt::QueuedConnection); //normally would have this schedule like once every 10 seconds max or whatever that formula was .... and it'd be checking for pending/confirmed payments etc. we are in this demo just generating a random number 1-10 and seeing if it's 3. lol
}
