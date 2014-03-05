#include <QCoreApplication>

#include "abc2dbinitializercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Abc2dbInitializerCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
