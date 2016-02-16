#include <QCoreApplication>

#include "antikeyandmouseloggercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AntiKeyAndMouseLoggerCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
