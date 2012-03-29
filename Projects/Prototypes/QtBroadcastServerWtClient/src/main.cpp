#include <Wt/WApplication>
using namespace Wt;

#include <QtCore/QCoreApplication>
#include <QThread>

#include "qtbroadcastservernumberwatcher.h"
#include "wtnumberwatchingwidget.h"

WApplication *handleNewWtConnection(const WEnvironment& env)
{
    return new WtNumberWatchingWidget(env);
}
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QThread broadcastServerThread;
    QtBroadcastServerNumberWatcher *qtServer = QtBroadcastServerNumberWatcher::Instance();
    qtServer->moveToThread(&broadcastServerThread);
    broadcastServerThread.start();

    //in the initial launch this won't matter... but if i ever stop and restart the server, there might exist a race condition where the init() event is not processed before the first client connects and tries to QtBroadcastServerConnect() (actually... since that too is an event... it won't matter. lol i'm dumb. disregard this)
    QMetaObject::invokeMethod(qtServer, "init", Qt::QueuedConnection);

    int ret = WRun(argc, argv, &handleNewWtConnection);

    QMetaObject::invokeMethod(qtServer, "stop", Qt::BlockingQueuedConnection); //blocking so it has time to do all it's deallocation n stuff
    QMetaObject::invokeMethod(&broadcastServerThread, "quit", Qt::QueuedConnection);
    broadcastServerThread.wait();

    return ret;
    return app.exec(); //<-- this is the line that i think needs to be called and won't be without the qtwithwt interop library :(. we'll never enter the Qt event loop :(. but MAYBE since i'm putting the singleton on it's own thread and starting it all from within this main function... just MAYBE it will work. no idea hence keep coding bitch


    //HMMMM maybe after we do a server.start(); we can then do an app.exec(); and then after that we can do a server.waitForShutdown(); (we'd need to exit via Qt::exit() or whatever it's called in order to get exec to return)
}
