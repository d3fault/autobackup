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
#include "simplesinglebackend1.h"

SimpleSingleBackend1::SimpleSingleBackend1(QObject *parent) :
    QObject(parent)
{
}
void SimpleSingleBackend1::threadStartingUp()
{
    //In general, it is safer to let the threads themselves create and destroy any objects they deal with. If we would have put the following line in our constructor, then it would have run from the GUI thread instead of our own backend thread. For POD types this usually doesn't matter, but for anything requiring a QObject *parent in it's constructor: it helps that 'this' is already on it's own thread before being passed in as *parent. I find that it's easier to create a mock constructor/destructor as slots and to call them myself. It can be hard to predict from what thread a constructor/destructor will actually run on
    m_Hasher = new QCryptographicHash(QCryptographicHash::Sha1);
}
void SimpleSingleBackend1::thrashHashStringNtimes(const QString &stringToThrashHash, int numTimesToHash)
{
    //This thrashing hashing method is really just meant to simulate load so you can see that the GUI remains responsive due to proper threading. If 'this' was still on the GUI thread, the application would lock up until our slot has finished

    //A slot on a backend thread should only contain a single unit of execution. That is, you should not queue any further slots (calling slots in this object directly (normal method call) is OK, however). If using QProcess, QAbstractSocket, QIODevice, or any other class with synchronous capabilities, those capabilities should be used to ensure a single unit of execution. The reason for this is simple: if you connect to an asynchronous class's finished() signal (all 3 of the classes mentioned have a similar signal) to a slot in this class, finished() will call that slot during a different 'single unit of execution' (whenever our event loop receives it). This is problematic when trying to achieve a clean shut down of backend objects running on a their own threads. Suppose your threadShuttingDown slot got called before the slot connected to the async finished() signal was processed? Even worse, what if the QThread::quit() message beats the finished() signal? Your slot connected to the finished() would never be called because the event loop is no longer running (and you are now possibly in a broken state and have lost data. Read and understand the comments in SimpleSingleFrontendWithTwoBackendsExampleTest::handleAboutToQuit() for more information on the subject). You could code very carefully around this with tons of "waitingForResponse" type booleans in your "threadShuttingDown" slot (and then you'd call the respective synchronous waitFor*() methods before allowing "threadShuttingDown" to continue), but it's a lot easier to just ensure that our object's state is in tact when this method returns back to the caller. Blocking is not a problem since we're on our own thread

    //QCryptographicHash expects a QByteArray, not a QString
    QByteArray stringAsArray;
    //It returns a QByteArray as well
    QByteArray hashResult;

    //These two numbers are here to limit the rate at which we update the GUI. When I first made this project, I didn't have the limiters in place and the GUI still locked up (despite us being on our own thread) because it was receiving way too many update events. This slows things down a bit... but as long as your actual backend threads are doing some serious work in between updating the GUI, you won't need limiters like this
    quint8 DEBUG_limiter = 0;
    const quint8 DEBUG_limiterEmitEvery = 100;

    for(int i = 0; i < numTimesToHash; ++i)
    {
        m_Hasher->reset();
        stringAsArray.clear();
        hashResult.clear();

        //Here we add our string, plus our current index (to give it some random'ness), to a QByteArray, the type QCryptographicHash operates on
        stringAsArray.append(stringToThrashHash + QString::number(i));

        //Here we add the QByteArray's contents to the hash's internal buffer. You can call this multiple times before calling result()
        m_Hasher->addData(stringAsArray);

        //Now hash all of the contents of the internal buffer, convert it to hex so it looks pretty in the GUI, and capture the result
        hashResult = m_Hasher->result().toHex();

        if(DEBUG_limiter == DEBUG_limiterEmitEvery)
        {
            DEBUG_limiter = 0;

            //Even though we are a 'single unit of execution', that doesn't mean we can't send out signals at any time we want. The GUI, running on it's own thread, will receive these signals/emits and update the GUI appropriately. Note: if your backend slot enters an infinite loop, you should periodically call QCoreApplication::processEvents() manually... or else your backend thread will never process the "threadShuttingDown" and QThread::quit() events. If you do take that approach, you should check for a bool m_IsQuitting that you set to true in "threadShuttingDown" right after the call to processEvents()... and then return or otherwise end the infinite loop
            emit hashGenerated(hashResult); //implicit conversion to QString
        }
        ++DEBUG_limiter;
    }
}
void SimpleSingleBackend1::threadShuttingDown()
{
    //Just like threadStartingUp, we want our actual thread to do it's own initialization and it's own destruction [of member objects]
    //Since m_Hasher is:
        //a) a pointer
        //b) new'd (on the heap)
        //c) NOT a child of a QObject
    //then we have to delete it manually. Note that there isn't really any good reason for m_Hasher to be a pointer/on-the-heap in this case. I only made it one so my threadStartingUp and threadShuttingDown methods wouldn't be empty :-P
    delete m_Hasher;
}
