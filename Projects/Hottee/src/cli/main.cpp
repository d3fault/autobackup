#include <QCoreApplication>
#include "hotteecli.h"

//TODOreq: devise solution so that node failures don't stop the chain. drive failures already don't, which might be good enough (unless it's the system drive!). psu failures will be most common prolly

//intended usage: app [netcat in] [netcat in args] [netcat out] [netcat out args] [dest1] [dest2]
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HotteeCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
