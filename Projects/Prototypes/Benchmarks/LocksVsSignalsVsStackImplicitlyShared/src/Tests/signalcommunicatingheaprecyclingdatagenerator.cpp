#include "signalcommunicatingheaprecyclingdatagenerator.h"

SignalCommunicatingHeapRecyclingDataGenerator::SignalCommunicatingHeapRecyclingDataGenerator(int loopCount, int sizeMultiplier, const QString &dataToBeMultiplied)
    : m_LoopCount(loopCount), m_SizeMultiplier(sizeMultiplier)
{
    m_StartingVal.append(dataToBeMultiplied);
}
void SignalCommunicatingHeapRecyclingDataGenerator::init()
{
    m_RecycleList = new QList<QString*>();
    m_Interval = 0;
    m_ReplaceTo.append("b");
    //m_StartingVal.append("wpqeiaiqmnaewar");
    m_Done = false;
    m_NumGenerated = 0;
}
void SignalCommunicatingHeapRecyclingDataGenerator::startTest()
{
    generateOne();
}
void SignalCommunicatingHeapRecyclingDataGenerator::generateOne()
{
    if(m_Interval < m_LoopCount)
    {
        QString *theBytes = getRecycledOrNew();
        //change every A to a B
        theBytes->replace("a", m_ReplaceTo);
        emit bytesGenerated(theBytes);
        ++m_Interval;
        //emit d(QString("on interval: ") + QString::number(m_Interval));
        QMetaObject::invokeMethod(this, "startTest", Qt::QueuedConnection);
    }
    else
    {
        //wtf are we measuring here? should we wait until the messages all get back into recycled before doing testFinished?
        //really, wtf are we measuring? the average message cycle length? the total time it takes to complete everything? from generator perspective (right here) or after processing too? isn't this technically going to be message thrashing? maybe i should set the interval lower to start. isn't that the point of this test because i have no fucking clue? well i definitely need something to measure first ahaha. ok fuck it ima keep track of how many are "new'd" and then just mark a bool done = true right here, then count them as they get recycled. the total number generated might be of interest too...
        //emit testFinished();
        //emit d("setting done to true");
        m_Done = true;
        checkDone();
    }
}
QString *SignalCommunicatingHeapRecyclingDataGenerator::getRecycledOrNew()
{
    if(!m_RecycleList->isEmpty())
    {
        //todo: maybe keep track of recycle vs new hits? this will slow down the tests though so not right now
        return m_RecycleList->takeLast();
    }
    QString *newString = new QString();
    ++m_NumGenerated;
    //emit d(QString("Num Generated: ") + QString::number(m_NumGenerated));
    for(int i = 0; i < m_SizeMultiplier; ++i)
    {
        newString->append(m_StartingVal);
    }
    return newString;
}
void SignalCommunicatingHeapRecyclingDataGenerator::recycledUsed(QString *used)
{
    m_RecycleList->append(used);
    //emit d(QString("recycling one. recycle list size: " + QString::number(m_RecycleList->size())));
    if(m_Done)
    {
        checkDone();
    }
}
void SignalCommunicatingHeapRecyclingDataGenerator::cleanup()
{
    int rCount = m_RecycleList->count();
    for(int i = 0; i < rCount; ++i)
    {
        QString *existing = m_RecycleList->at(i);
        delete existing;
    }
    m_RecycleList->clear();
    delete m_RecycleList;
}
void SignalCommunicatingHeapRecyclingDataGenerator::checkDone()
{
    //emit d(QString("now gathering recycled: ") + QString::number(m_RecycleList->count()) + QString(" of ") + QString::number(m_NumGenerated));
    if(m_RecycleList->count() >= m_NumGenerated)
    {
        emit testFinished(m_NumGenerated, (m_NumGenerated * (m_SizeMultiplier * m_StartingVal.length())));
    }
}
