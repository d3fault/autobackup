#include <QCoreApplication>

#include "abc2transactioncreditorcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Abc2TransactionCreditorCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
