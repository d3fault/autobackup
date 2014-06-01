#include <QCoreApplication>

#include "lastmodifiedtimestampstoolscli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LastModifiedTimestampsToolsCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
