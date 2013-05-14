#include <QtGui/QApplication>

#include "rpcbankserverclienttest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RpcBankServerClientTest bankServerClientTest;
    Q_UNUSED(bankServerClientTest)

    //BankClientDebugWidget w(&bankServerClientTest);
    //w.show();

    return a.exec();
}
