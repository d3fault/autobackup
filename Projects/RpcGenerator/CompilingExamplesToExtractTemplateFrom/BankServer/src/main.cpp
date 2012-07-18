#include <QtGui/QApplication>
#include "bankservertest.h"
#include "bankserverdebugwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BankServerTest bankServerTest;

    bankServerDebugWidget w(&bankServerTest);
    w.show();

    return a.exec();
}
