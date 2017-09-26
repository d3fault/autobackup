#include <QCoreApplication>

#include "stupidkeyvaluecontenttracker.h"
#include "stupidkeyvaluecontenttrackercli.h"

//modelled LOOSELY around git, but key/value store instead of fs. fs is so 1990s ;oP
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    StupidKeyValueContentTrackerCli cli;
    StupidKeyValueContentTracker stupidKeyValueContentTracker;
    StupidKeyValueContentTracker::establishConnectionsToAndFromBackendAndUi<StupidKeyValueContentTrackerCli>(&stupidKeyValueContentTracker, &cli);
    cli.main();

    return a.exec();
}
