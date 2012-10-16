#include "cleanobserverpatternexamplebackendb.h"

CleanObserverPatternExampleBackendB::CleanObserverPatternExampleBackendB(QObject *parent) :
    QObject(parent), m_MaxNumberToGenerate(10), m_NumberToWatchFor(3)
{
}
void CleanObserverPatternExampleBackendB::threadStartingUp()
{
    //see: CleanObserverPatternExampleBackendA::threadStartingUp()
    //m_Timer is deleted automatically for us because 'this' is set as it's parent. However, since it's hard to predict what thread we'll be on when our destructor is called, we're going to delete it manually in threadShuttingDown() just to be safe
    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(handleTimerTimedOut()));
    m_Timer->start(1000);
}
void CleanObserverPatternExampleBackendB::updateRandomNumberGeneratorProperties(int maxNum, int numberToWatchFor)
{
    m_MaxNumberToGenerate = maxNum;
    m_NumberToWatchFor = numberToWatchFor;
}
void CleanObserverPatternExampleBackendB::handleTimerTimedOut()
{
    //For this back-end object, we are periodically timing out, generating a single random number, and then emitting different results depending on if it's what we want or not. This handleTimerTimedOut() slot is what should be considered the 'single unit of execution' (see: CleanObserverPatternExampleBackendA::thrashHashStringNtimes() ). We should not be in an indeterminate or pending state when the method returns (though that isn't strictly necessary, if you're careful)

    //Generate the random number. The percent sign is a modulus operator and when used in this fashion, ensures that the result will be less than m_MaxNumberToGenerate
    int randomNum = qrand() % m_MaxNumberToGenerate;
    if(randomNum == m_NumberToWatchFor)
    {
        //Hack ;-)
        if(randomNum == 3)
        {
            emit randomNumberResultsGathered(QString("d") + QString::number(randomNum) + QString("fault is god"));
        }
        else
        {
            emit randomNumberResultsGathered(QString("We found the number we are looking for: ") + QString::number(randomNum));
        }
    }
    else
    {
        emit randomNumberResultsGathered(QString("bah, ") + QString::number(randomNum) + QString(" isn't what I am looking for"));
    }

}
void CleanObserverPatternExampleBackendB::threadShuttingDown()
{
    //see: CleanObserverPatternExampleBackendA::threadShuttingDown()
    //Not strictly necessary, but ensures us that we're on our backend thread when called
    m_Timer->stop();
    delete m_Timer;
}
