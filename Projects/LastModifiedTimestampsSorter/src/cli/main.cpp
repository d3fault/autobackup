#include <QCoreApplication>

#include "lastmodifiedtimestampssortercli.h"

//takes filename input as arg, writes sorted by timestamp contents to stdout
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LastModifiedTimestampsSorterCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
