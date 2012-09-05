#include <QtGui/QApplication>
#include "mainwidget.h"

#include "robustnetworkmessagingforrpcgeneratortest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RobustNetworkMessagingForRpcGeneratorTest test;

    mainWidget w(&test);
    w.show();

    return a.exec();
}
