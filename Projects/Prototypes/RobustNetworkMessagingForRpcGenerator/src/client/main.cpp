#include <QtGui/QApplication>
#include "mainwidget.h"

#include "robustnetworkmessagingforrpcgeneratortestclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RobustNetworkMessagingForRpcGeneratorTestClient testClient;

    mainWidget w(&testClient);
    w.show();

    return a.exec();
}
