#include "osiosmainwindow.h"
#include <QApplication>

//TODOoptimization: the "current working state" of the application, when saved at quit, should be a single file. this has nothing to do with whatever formats/etc are used for persisting user data, but will include subsets of those (as well as various user-specific application settings (those too should be persisted though!)). Basically it's just a cache generated on exit so that RANDOM disk reads are minimized on next launch. For example this could contain the conversations in the conversatinos window (who), including the conversations themselves (but they are chopped off by the qplaintextedit to 6900 currently)... but also things such as "last used tab" so it jumps right back to where you were
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OsiosMainWindow w; //TODOoptional: I'm only defining the widgets and layouts for now, but ideally they will bind to backend/abstracted objects and the entire app can have an identical Cli counter-part. Might prove difficult though (lynx is gpl btw fml), since this is largely a GUI-centric app. If abstracted properly, a website version could be done with ease (ad supported) could be done too (but then the concept of "local machine" vanishes :-/ (unless i use those local storage javascript shits, bleh)))
    w.show();

    return a.exec();
}
