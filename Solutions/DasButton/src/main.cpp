#include <QCoreApplication>

#include "dasbuttoncli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DasButtonCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
