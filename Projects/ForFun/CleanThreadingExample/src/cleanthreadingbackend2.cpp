#include "cleanthreadingbackend2.h"

CleanThreadingBackend2::CleanThreadingBackend2(QObject *parent) :
    QObject(parent), m_MaxNumberToGenerate(10), m_NumberToWatchFor(3)
{
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(handleTimerTimedOut()));
    m_Timer.start(1000);
}
void CleanThreadingBackend2::updateRandomNumberGeneratorProperties(int maxNum, int numberToWatchFor)
{
    //Unlike CleanThreadingBackend1, our GUI can change the random number generator's properties at any time. The GUI sanitizes the input, so we don't have to. If the GUI didn't, this would be a great spot for us to do it
    m_MaxNumberToGenerate = maxNum;
    m_NumberToWatchFor = numberToWatchFor;
}
void CleanThreadingBackend2::handleTimerTimedOut()
{
    //For this backend object, we are periodically timing out, generating a single random number, and then emitting different results depending on if it's what we want or not. This handleTimerTimedOut() slot is what should be considered the 'single unit of execution' (see: CleanThreadingBackend1::thrashHashStringNtimes() ). We should not be in an indeterminate or pending state when the method returns (though that isn't strictly necessary, if you're careful)

    //Generate the random number. The percent sign is a modulus operator and when used in this fashion, ensures that the result will be less than m_MaxNumberToGenerate. The "+1" just makes it inclusive (ie, we can generate our m_MaxNumberToGenerate.. not just anything below it)
    int randomNum = qrand() % (m_MaxNumberToGenerate+1);
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
