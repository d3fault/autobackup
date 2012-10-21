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

#include "simplesinglefrontendwithtwobackendsexampletest.h"

int main(int argc, char *argv[])
{
    //Required for Qt Gui (QWidgets etc) -- use the base class QCoreApplication instead if you want a command line application (also add QT += console to your .pro file)
    QApplication a(argc, argv);

    //Here we allocate our controller of sorts on the stack, and then start it. It does not stop until it's destructor is called, which happens after the call below to a.exec() returns. QCoreApplication::exec() only returns when the application has been instructed to exit (closing the last window, shutting down your OS, or calling QCoreApplication::quit() from anywhere in your application)
    SimpleSingleFrontendWithTwoBackendsExampleTest exampleTest;

    //and then start it. This method call isn't strictly necessary: it's contents could go in SimpleSingleFrontEndWithTwoBackendsExampleTest's constructor. However, I want to call at least one method on my local variable so that the compiler doesn't complain about it being unused. Another way you could suppress the compiler warning is move the contents to the constructor and then putting Q_UNUSED(exampleTest) for the next line instead of calling startExample(). It makes no difference
    exampleTest.startExample();
    
    //Enters Qt's event loop, the real magic behind signals and slots. Does not leave until instructed to do so
    return a.exec();
}
