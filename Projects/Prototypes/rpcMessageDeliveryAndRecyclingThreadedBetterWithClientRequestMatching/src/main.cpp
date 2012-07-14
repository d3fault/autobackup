#include <QtGui/QApplication>

#include "debugstartstopgui.h"
#include
"rpcmessagedeliveryandrecyclingthreadedbetterwithclientrequestmatchingtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    rpcMessageDeliveryAndRecyclingThreadedBetterWithClientRequestMatchingTest
rpcMessageTest;

    DebugStartStopGui w(&rpcMessageTest);
    w.show();

    return a.exec();
}
