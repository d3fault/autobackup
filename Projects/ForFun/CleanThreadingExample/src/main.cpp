/*
Copyright (c) 2012, d3fault <d3faultdotxbe@gmail.com>
Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
*/
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
