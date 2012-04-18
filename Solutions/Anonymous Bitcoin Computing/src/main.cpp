#include <Wt/WApplication>
using namespace Wt;

#include "database.h"
#include "anonymousbitcoincomputing.h"

WApplication *serverEntryPoint(const WEnvironment &env)
{
    return new AnonymousBitcoinComputing(env);
}
int main(int argc, char **argv)
{
    Database::GlobalConfigure();

    //Database processWideDb; //so sqlite doesn't close the file while there are no users or something? the dbo tutorial says to do this. i was originally planning to give this to Qt to use... but not anymore now that the AppDb is an SSL session away~

    AppDbHelper *appDbHelper = AppDbHelper::Instance();
    //the Qt "AppDbHelper" has an in-memory cache, and a qt sqlite cache (because using a bunch of memory will limit our connections amount!). the actual AppDb pushes all updates to us... so we only need to get the 'latest and greates' (this could be really inefficient later on) right at startup.......... OR we can just pull if it doesn't exist in our own cache... but if it is in our cache, we know it's up-to-date because we always get pushed updates (this is much more efficient)
    //i want to throw up at how complicated this is getting

    return WRun(argc, argv, &serverEntryPoint);

    /*
    QCoreApplication app(argc, argv);

    QThread broadcastServerThread;
    QtBroadcastServerNumberWatcher *qtServer = QtBroadcastServerNumberWatcher::Instance();
    qtServer->moveToThread(&broadcastServerThread);
    broadcastServerThread.start();

    //in the initial launch this won't matter... but if i ever stop and restart the server, there might exist a race condition where the init() event is not processed before the first client connects and tries to QtBroadcastServerConnect() (actually... since that too is an event... it won't matter. lol i'm dumb. disregard this)
    QMetaObject::invokeMethod(qtServer, "init", Qt::BlockingQueuedConnection); //i could even use a return value to make sure that it init'd ok and check it before doing WRun...

    int ret = WRun(argc, argv, &handleNewWtConnection);

    QMetaObject::invokeMethod(qtServer, "stop", Qt::BlockingQueuedConnection); //blocking so it has time to do all it's deallocation n stuff
    broadcastServerThread.quit();
    broadcastServerThread.wait();

    qDebug() << "last thread stopped. return code: " << QString::number(ret);

    return ret;
      */
}
