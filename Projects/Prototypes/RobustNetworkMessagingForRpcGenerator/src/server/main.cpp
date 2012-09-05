#include <QtGui/QApplication>
#include "mainwidget.h"

#include "robustnetworkmessagingforrpcgeneratortestserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RobustNetworkMessagingForRpcGeneratorTestServer testServer;

    mainWidget w(&testServer);
    w.show();

    return a.exec();
}
