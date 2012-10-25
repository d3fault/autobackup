#include "cleanthreadingbackend1.h"

CleanThreadingBackend1::CleanThreadingBackend1(QObject *parent) :
    QObject(parent), m_Hasher(QCryptographicHash::Sha1)
{ }
void CleanThreadingBackend1::thrashHashStringNtimes(const QString &stringToThrashHash, int numTimesToHash)
{
    //This thrashing hashing method is really just meant to simulate load so you can see that the GUI remains responsive due to proper threading. If we were still on the GUI thread, the application would lock up until our slot has finished

    //A slot on a backend thread should only contain a single unit of execution. That is, you should not queue any further slots (calling slots/methods on ourself directly is OK, however). If using QProcess, QAbstractSocket, QIODevice, or any other class with synchronous capabilities, those capabilities should be used to ensure a single unit of execution. The reason for this is simple: if you connect to an asynchronous class's finished() signal (all 3 of the Qt classes mentioned above have a similar signal) to a slot in this class, finished() will call that slot during a different 'single unit of execution' (whenever our event loop receives it). This is problematic when trying to achieve a clean shut down of backend objects running on a their own thread. Suppose you QThread::quit() before the slot connected to the async finished() signal was processed? Your slot connected to the finished() would never be called because the event loop is no longer running (and you are now possibly in a broken state and have lost data. Read and understand the comments in CleanThreadingExampleTest::cleanupBackendObjectsIfNeeded() for more information on the subject). You could code very carefully around this with tons of "waitingForResponse" type booleans in your destructor (and then you'd call the respective synchronous waitFor*() methods before continuing), but it's a lot easier to just ensure that our object's state is in tact when this method returns back to the caller. Blocking is not a problem since we're on our own thread

    //QCryptographicHash expects a QByteArray, not a QString
    QByteArray stringAsArray;

    //It returns a QByteArray as well
    QByteArray hashResult;

    //These two numbers are here to limit the rate at which we update the GUI. When I first made this project, I didn't have the limiters in place and the GUI still locked up (despite us being on our own thread) because it was receiving way too many signals too frequently (the GUI was locking up because it was spending all it's time processing them). This limiter slows things down a bit... but as long as your actual backend threads are doing some real work in between updating the GUI, you won't need limiters like this. It should be a trivial exercise to take them out (else, why are you reading this?)
    quint8 DEBUG_limiter = 0;
    const quint8 DEBUG_limiterEmitEvery = 100;

    for(int i = 0; i < numTimesToHash; ++i)
    {
        m_Hasher.reset();
        stringAsArray.clear();
        hashResult.clear();

        //Here we add our string, plus our current index (to give it some random'ness), to a QByteArray, the type QCryptographicHash operates on
        stringAsArray.append(stringToThrashHash + QString::number(i));

        //Here we add the QByteArray's contents to the hash's internal buffer. You can call this multiple times before calling result()
        m_Hasher.addData(stringAsArray);

        //Now hash all of the contents of the internal buffer, convert it to hex so it looks pretty in the GUI, and capture the result
        hashResult = m_Hasher.result().toHex();

        if(DEBUG_limiter == DEBUG_limiterEmitEvery)
        {
            DEBUG_limiter = 0;

            //Even though we are a 'single unit of execution', that doesn't mean we can't send out signals at any time we want. The GUI, running on it's own thread, will receive these signals and updates itself appropriately. Note: if your backend slot enters an infinite loop, you should periodically call QCoreApplication::processEvents() manually... or else your backend thread will never process the QThread::quit() event or any signals sent to your other slots. If you do take that approach, you should check this->thread()->isRunning() right after the call to processEvents()... and then return or otherwise end the infinite loop if it's false. Shutting down the thread via QThread::quit() only tells the thread to quit. Since you're in an infinite loop calling processEvents() manually, you have the power to ignore it (this is NOT advised). If QThread::terminate() is called, the thread will be terminated instantly, possibly losing/corrupting data. This comment is off-topic for our scenario: we are a finite slot, so we do not need to call processEvents()
            emit hashGenerated(hashResult); //implicit conversion to QString
        }
        ++DEBUG_limiter;
    }
}
