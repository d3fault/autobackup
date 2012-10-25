#include <QtGui/QApplication>

#include "cleanthreadingexampletest.h"

int main(int argc, char *argv[])
{
    //Required for Qt Gui (QWidgets etc) -- use the base class QCoreApplication instead if you want a command line application (also add QT += console to your .pro file)
    QApplication app(argc, argv);

    //Here we allocate our controller of sorts on the stack. It starts right away and does not stop until either it's destructor is called or QCoreApplication::aboutToQuit() is emitted (we check to see if we need to stop at both places, for added safety). aboutToQuit() is emitted just before app.exec() is finished, and the destructor is called right before main() goes out of scope. QCoreApplication::exec() only returns (normally, see caveat for Windows below) when the application has been instructed to exit (closing the last window, shutting down your OS, or calling QCoreApplication::quit() from anywhere in your application)
    CleanThreadingExampleTest test;
    Q_UNUSED(test); //Hides a compiler warning about an unused variable
    
    //Enters Qt's event loop, the real magic behind signals and slots. Does not leave until instructed to do so (except on Windows, where processes terminate all-of-the-sudden when the OS is shutting down rofl. For that reason (and others), we have to listen to QCoreApplication::aboutToQuit() so that we can shutdown cleanly on inferior operating systems as well)
    return app.exec();
}
