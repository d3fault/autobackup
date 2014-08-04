#include <QCoreApplication>

#include "directmethodcallvsvirtualmethodcallvsinvokemethodvssignalslotactivationcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
