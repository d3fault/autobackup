#include <QApplication>

#include "couchbasenotepadgui.h"

//Err shit this design is fatally flawed because libevent takes over the thread. my get/store signal/slot stuff won't get processed until I processEvents as a libevent timer event (30ms)... which means I'll have a 30ms on average latency for both get or add to even begin being processed by libcouchbase. Fuck it though, efficiency is not my goal. I think the better solution is to have a "same thread" get/store api that is qt-ish (signals/slots) and that uses internally (by requirement) the libevent stuff to communicate with the other thread
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CouchbaseNotepadGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
