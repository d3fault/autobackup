#include <QCoreApplication>
#include "hotteecli.h"

//TODOreq: don't overwrite targets. maybe at the start I create a subfolder using the current datetime
//TO DOnereq: devise solution so that node failures don't stop the chain. drive failures already don't, which might be good enough (unless it's the system drive!). psu failures will be most common prolly
//^^need to listen to output process finishing, then be able to trigger a re-start of that process at which point the 100mb segment number is presented to the user for typing into the now-rejoining chain. the user failing to start the netcat receiving process on the remote machine and the netcat sending process locally (the "optional process out" of this app) will likely fill a buffer and overlow at some point, so the user must type this number in within reasonable speed until a proper implementation can be written (perhaps just a simple timeout, but perhaps netcat's internal timeout takes care of this already)

//intended usage: app <netcat in args> <dest1> <dest2> [netcat out args] [filename offset to jump to]
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HotteeCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
