#include <QCoreApplication>

#include "singlesignalmultipleslotscli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SingleSignalMultipleSlotsCli cli;
    Q_UNUSED(cli)
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);

    return a.exec();
}
