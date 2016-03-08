#include <QCoreApplication>

#include "qtsystemsignalhandlercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtSystemSignalHandlerCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
