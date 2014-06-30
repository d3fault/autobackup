#include <QCoreApplication>

#include "simpleasynclibraryslotinvokeandsignalresponsecli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SimpleAsyncLibrarySlotInvokeAndSignalResponseCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
