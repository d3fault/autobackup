#include "mutexusingheaprecyclingdatagenerator.h"

MutexUsingHeapRecyclingDataGenerator::MutexUsingHeapRecyclingDataGenerator(int loopCount, int sizeMultiplier, const QString &dataSample)
    : m_LoopCount(loopCount), m_SizeMultiplier(sizeMultiplier)
{
    m_StartingVal.append(dataSample);
}
void MutexUsingHeapRecyclingDataGenerator::init()
{
    m_RecycleList = new QList<QString*>();
    m_Interval = 0;
    m_ReplaceTo.append("b");
    m_CountChar.append("a");
    m_Done = false;
    m_NumGenerated = 0;
}
void MutexUsingHeapRecyclingDataGenerator::startTest()
{
    generateOne();
}
void MutexUsingHeapRecyclingDataGenerator::generateOne()
{
    while(m_Interval < m_LoopCount) //changing this to a while loop instead of doing a message based while loop halved the time. we can't do a regular while loop for the message based recycling, as it'd finish the loop before ever processing a recycle message. we can do it for the stack one though. basically what i'm getting at is that this test is flawed because the generation of data should not be an event while loop... but MAYBE if all 3 of them are then the results are accurate. i'm unsure.
    {
        QString *theBytes = getRecycledOrNew();
        //theBytes->replace("a", m_ReplaceTo);
        m_Count = theBytes->count(m_CountChar);
        emit bytesGenerated(theBytes);
        ++m_Interval;
        //QMetaObject::invokeMethod(this, "startTest", Qt::QueuedConnection);
    }
    //else
    //{
        m_Done = true;
        checkDone();
    //}
}
QString * MutexUsingHeapRecyclingDataGenerator::getRecycledOrNew()
{
    QMutexLocker scopedLock(&m_RecycleLock);

    if(!m_RecycleList->isEmpty())
    {
        return m_RecycleList->takeLast();
    }
    QString *newString = new QString();
    ++m_NumGenerated;
    for(int i = 0; i < m_SizeMultiplier; ++i)
    {
        newString->append(m_StartingVal);
    }
    return newString;
}
void MutexUsingHeapRecyclingDataGenerator::recycledUsed(QString *used)
{
    m_RecycleLock.lock();

    m_RecycleList->append(used);

    m_RecycleLock.unlock();

    if(m_Done)
    {
        checkDone();
    }
}
void MutexUsingHeapRecyclingDataGenerator::checkDone()
{
    QMutexLocker scopedLock(&m_RecycleLock);

    if(m_RecycleList->count() >= m_NumGenerated)
    {
        emit testFinished(m_NumGenerated, (m_NumGenerated * (m_SizeMultiplier * m_StartingVal.length())));
    }
}
void MutexUsingHeapRecyclingDataGenerator::cleanup()
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
