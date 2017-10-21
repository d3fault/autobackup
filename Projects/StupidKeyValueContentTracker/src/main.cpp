#include <QCoreApplication>

#include "stupidkeyvaluecontenttracker.h"
#include "stupidkeyvaluecontenttrackercli.h"

//modelled LOOSELY around git, but key/value store instead of fs. fs is so 1990s ;oP. git is also designed/coded as a "monolithic cli" first, and SEPARATE lib (libgit2) later; libgit2 isn't even an "official" lib I don't think. duplicate code leads to bugs leads to data loss leads to suicide
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    StupidKeyValueContentTrackerCli cli;
    StupidKeyValueContentTracker stupidKeyValueContentTracker;
    StupidKeyValueContentTracker::establishConnectionsToAndFromBackendAndUi<StupidKeyValueContentTrackerCli>(&stupidKeyValueContentTracker, &cli);
    cli.main();

    return a.exec();
}
