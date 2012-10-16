#include <QtGui/QApplication>

#include "qwidgetcleanobserverpatternexampletest.h"

int main(int argc, char *argv[])
{
    //Required for Qt Gui (QWidgets etc) -- use the base class QCoreApplication instead if you want a command line application (also add QT += console to your .pro file)
    QApplication a(argc, argv);

    //Here we allocate our controller of sorts on the stack, and then start it. It does not stop until it's destructor is called, which happens after the call below to a.exec() returns. QCoreApplication::exec() only returns when the application has been instructed to exit (closing the window, shutting down your OS, or calling QCoreApplication::quit() from anywhere in your application)
    QWidgetCleanObserverPatternExampleTest exampleTest;

    //and then start it. This method call isn't strictly necessary: it's contents could go in QWidgetCleanObserverPatternExampleTest's constructor. However, I want to call at least one method on my local variable so that the compiler doesn't complain about it being unused
    exampleTest.startExample();
    
    //Enters Qt's event loop, the real magic behind signals and slots. Does not leave until instructed to do so
    return a.exec();
}
