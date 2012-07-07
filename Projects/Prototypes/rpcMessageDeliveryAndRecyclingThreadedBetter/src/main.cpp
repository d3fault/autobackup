#include <QtGui/QApplication>

#include "debugstartstopgui.h"
#include "rpcmessagedeliveryandrecyclingthreadedbettertest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    rpcMessageDeliveryAndRecyclingThreadedBetterTest rpcMessageTest;

    DebugStartStopGui w(&rpcMessageTest);
    w.show();

    return a.exec();
}
