#include <QCoreApplication>

#include "abc2withdrawrequestprocessorcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Abc2WithdrawRequestProcessorCli cli;
    Q_UNUSED(cli)

    return a.exec();
}
