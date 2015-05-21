#include <QCoreApplication>

#include "rpcgeneratorcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RpcGeneratorCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
