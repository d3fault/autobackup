#include <QtGui/QApplication>

#include "bankserverclienttest.h"
#include "bankclientdebugwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BankServerClientTest bankServerClientTest;

    BankClientDebugWidget w(&bankServerClientTest);
    w.show();

    return a.exec();
}
