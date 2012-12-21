#include <QtGui/QApplication>
#include "rpcbankservertest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RpcBankServerTest rpcBankServerTest;
    Q_UNUSED(rpcBankServerTest)

    return a.exec();
}
