#include <QCoreApplication>

#include "abc2hugebitcoinkeylistpageaddercli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Abc2hugeBitcoinKeyListPageAdderCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
