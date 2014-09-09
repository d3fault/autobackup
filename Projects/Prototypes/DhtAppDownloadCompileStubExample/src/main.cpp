#include <QCoreApplication>

#include "dhtappdownloadcompilestubexample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DhtAppDownloadCompileStubExample dht;
    Q_UNUSED(dht)

    return a.exec();
}
