#include <QCoreApplication>

#include "qthttpservercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtHttpServerCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
