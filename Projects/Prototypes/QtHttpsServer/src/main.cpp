#include <QCoreApplication>

#include "qthttpsservercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtHttpsServerCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
