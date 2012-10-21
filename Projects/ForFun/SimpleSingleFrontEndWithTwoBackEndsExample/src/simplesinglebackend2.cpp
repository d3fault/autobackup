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
#include "simplesinglebackend2.h"

SimpleSingleBackend2::SimpleSingleBackend2(QObject *parent) :
    QObject(parent), m_MaxNumberToGenerate(10), m_NumberToWatchFor(3)
{
}
void SimpleSingleBackend2::threadStartingUp()
{
    //see: SimpleSingleBackend1::threadStartingUp()
    //m_Timer is deleted automatically for us because 'this' is set as it's parent. However, since it's hard to predict what thread we'll be on when our destructor is called, we're going to delete it manually in threadShuttingDown() just to be safe... at which point it will remove itself as a child of 'this'. The worst case scenario (if for some reason it didn't remove itself as a child of 'this') is that it gets deleted twice... which is explicitly allowed in the C++ specification. The second delete operation does nothing. Read the QObject documentation to understand this parent/child relationship
    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(handleTimerTimedOut()));
    m_Timer->start(1000);
}
void SimpleSingleBackend2::updateRandomNumberGeneratorProperties(int maxNum, int numberToWatchFor)
{
    //Unlike SimpleSingleBackend1, our GUI can change the random number generator's properties at any time. The GUI sanitizes the input, so we don't have to. If the GUI didn't, this would be a great spot for us to do it
    m_MaxNumberToGenerate = maxNum;
    m_NumberToWatchFor = numberToWatchFor;
}
void SimpleSingleBackend2::handleTimerTimedOut()
{
    //For this backend object, we are periodically timing out, generating a single random number, and then emitting different results depending on if it's what we want or not. This handleTimerTimedOut() slot is what should be considered the 'single unit of execution' (see: SimpleSingleBackend1::thrashHashStringNtimes() ). We should not be in an indeterminate or pending state when the method returns (though that isn't strictly necessary, if you're careful)

    //Generate the random number. The percent sign is a modulus operator and when used in this fashion, ensures that the result will be less than m_MaxNumberToGenerate. The "+1" just makes it inclusive (ie, we can generator our maximum number to generate.. not just anything less than it)
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
void SimpleSingleBackend2::threadShuttingDown()
{
    //see: SimpleSingleBackend1::threadShuttingDown()
    //Not strictly necessary, but ensures us that we're on our backend thread when called
    m_Timer->stop();
    delete m_Timer;
}
