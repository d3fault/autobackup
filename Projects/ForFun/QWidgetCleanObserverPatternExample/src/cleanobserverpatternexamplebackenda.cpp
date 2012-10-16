#include "cleanobserverpatternexamplebackenda.h"

CleanObserverPatternExampleBackendA::CleanObserverPatternExampleBackendA(QObject *parent) :
    QObject(parent)
{
}
void CleanObserverPatternExampleBackendA::threadStartingUp()
{
    //In general, it is safer to let the threads themselves create and destroy any objects they deal with. If we would have put the following line in our constructor, then it would have run from the GUI thread instead of our own backend thread. For POD types this usually doesn't matter, but for anything requiring a QObject *parent in it's constructor: it helps that 'this' is already on it's own thread before being passed in as *parent. I find that it's easier to create a mock constructor/destructor as slots and to call them myself. It can be hard to predict from what thread a constructor/destructor will actually run on
    m_Hasher = new QCryptographicHash(QCryptographicHash::Sha1);
}
void CleanObserverPatternExampleBackendA::thrashHashStringNtimes(const QString &stringToThrashHash, int numTimesToHash)
{
    //This thrashing hashing method is really just meant to simulate load so you can see that the GUI remains responsive due to proper threading. If 'this' was still on the GUI thread, the application would lock up until our slot has finished

    //A slot on a backend thread should only contain a single unit of execution. That is, you should not queue any further slots (calling slots in this object directly (normal method call) is OK, however). If using QProcess, QAbstractSocket, QIODevice, or any other class with synchronous capabilities, those capabilities should be used to ensure a single unit of execution. The reason for this is simple: if you connect an asynchronous class's finished() signal (all 3 of those classes have something similar) to a slot in this class, finished() will call that slot during a different 'single unit of execution' (whenever the event loop receives it). This is problematic when trying to achieve clean shut downs of backend objects running on a separate thread. Suppose your threadShuttingDown slot got called in the meantime (before finished() was emitted)? Even worse, what if the QThread::quit() message beats the finished() signal? Your slot connected to finished() will never be called because the event loop is no longer running (and you are now possibly in a broken state and have lost data. Read and understand the comments in QWidgetCleanObserverPatternExampleTest::handleAboutToQuit() for more information on the subject). You could code very carefully around this, but it's a lot easier to just ensure that our object's state is in tact when this method returns back to the caller. Blocking is not a problem since we're on our own thread

    for(int i = 0; i < numTimesToHash; ++i)
    {
        m_Hasher->reset();

        //convert the QString to a QByteArray, what QCryptographicHash expects
        QByteArray stringAsArray;
        stringAsArray.append(stringToThrashHash + QString::number(i));

        //Here we add our string, plus our current index (to give it some random'ness), to the hash's internal buffer
        m_Hasher->addData(stringAsArray);

        //Even though we are a 'single unit of execution', that doesn't mean we can't send out signals at any time we want. The GUI, running on it's own thread, will receive these signals/emits and update the GUI appropriately. The bulk of the work, QCryptographicHash::addData() and QCryptographicHash::result(), is performed on our backend thread so as not to lock up the GUI thread
        emit hashGenerated(m_Hasher->result().toHex());
    }
}
void CleanObserverPatternExampleBackendA::threadShuttingDown()
{
    //Just like threadStartingUp, we want our actual thread to do it's own initialization and it's own destruction [of member objects]
    //Since m_Hasher is:
        //a) a pointer
        //b) new'd (on the heap)
        //c) NOT a child of a QObject
    //then we have to delete it manually. Note there isn't really any good reason for m_Hasher to be a pointer/on-the-heap in this case. I only made it one so my threadStartingUp and threadShuttingDown methods wouldn't be empty :-P
    delete m_Hasher;
}
