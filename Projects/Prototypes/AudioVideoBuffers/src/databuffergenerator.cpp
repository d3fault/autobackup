#include "databuffergenerator.h"

DataBufferGenerator::DataBufferGenerator(int totalMaxSizeOfAllGeneratedDataBuffers, int generatedDataBufferSize)
    : m_TotalMaxSizeOfAllGeneratedDataBuffers(totalMaxSizeOfAllGeneratedDataBuffers), m_GeneratedDataBufferSize(generatedDataBufferSize), m_ScheduleWhenReUseSubmitted(false)
{
    m_ReUseQueueMutex = new QQueue<GeneratedDataBuffer*>();
    m_UsedQueue = new QQueue<GeneratedDataBuffer*>();
}
//rush dangerous is true when we need to allocate one as soon as possible and even use our own thread to modify how many queue items there are.
//it is intended to be true only when we are doing a BlockingQueued call on scheduleBufferDelivery
//our direct connection to it as it waits for us to finish doing buffer stuff in our own thread as well as the hack/to-make-it-synchronous of using BlockingQueued connection for the perfect "wait" call that is easy as fuck to manage
void DataBufferGenerator::scheduleBufferDelivery(QObject objectToDeliverTo, bool rushDangerous)
{
    //do something (figure it out)
    //allocate if we need to
    //since we are called only using a Queued connection and we live on our own thread, we can be sure that our Queues don't need mutexes
    //even though we are called on our own thread, another thread is waiting on our return from this method.
    GeneratedDataBuffer *theGeneratedDataBuffer;
    if(m_ReUseQueue->size() > 0)
    {
        theGeneratedDataBuffer = m_ReUseQueue->dequeue();
    }
    else
    {
        int sizeUsed = m_UsedQueue->size() * m_GeneratedDataBufferSize;
        if(sizeUsed >= m_TotalMaxSizeOfAllGeneratedDataBuffers)
        {
            //how do we block here until we get a buffer?
            m_ScheduleWhenReUseSubmitted = true;
            m_LastToAskForABuffer = objectToDeliverTo;
            return;
            //TODO: should i be letting them be scheduling a send here and send it later when the ReUsed buffer fills up a bit? that would imply that this DataBufferGenerator is on it's own thread. currently it's thread-apathetic and can be accessed by multiple threads. i just don't know what design is better
        }
        if((m_TotalMaxSizeOfAllGeneratedDataBuffers - sizeUsed) > m_GeneratedDataBufferSize) //if we have enough free space to allocate one more buffer, do it.
        {
            theGeneratedDataBuffer = new GeneratedDataBuffer(m_GeneratedDataBufferSize);
            connect(newGeneratedDataBuffer, SIGNAL(doneUsing(GeneratedDataBuffer*)), this, SLOT(reclaim(GeneratedDataBuffer*)), Qt::QueuedConnection); //this would imply this running on it's own thread?
            //also, i might not need to send GeneratedDataBuffer in my doneUsing signal, they might be able to see who it's from just by looking at sender()
        }
    }
    //then
    m_UsedQueue->append(theGeneratedDataBuffer);
    //QMetaObject::invokeMethod(objectToDeliverTo, "hereIsYourBuffer", (rushDangerous ? Qt::DirectConnection : Qt::QueuedConnection), Q_ARG(GeneratedDataBuffer*,theGeneratedDataBuffer));
    sendBufferReadySignal(objectToDeliverTo, theGeneratedDataBuffer, rushDangerous);
}
void DataBufferGenerator::reclaim(GeneratedDataBuffer *selfPointer)
{
    //this slot is called on our thread when any thread calls emit this->doneUsing(this);
    int selfIndex = m_UsedQueue->indexOf(selfPointer);
    if(selfIndex < 0)
    {
        qDebug("Error reclaiming, didn't find self in used queue");
        return;
    }
    m_UsedQueue->removeAt(selfIndex);

    if(m_ScheduleWhenReUseSubmitted)
    {
        m_ScheduleWhenReUseSubmitted = false;
        m_UsedQueue->append(selfPointer);

        //QMetaObject::invokeMethod(m_LastToAsk, "hereIsYourBuffer", Qt::QueuedConnection, Q_ARG(GeneratedDataBuffer*,selfPointer));
        sendBufferReadySignal(m_LastToAskForABuffer, selfPointer, false); //we don't use rush dangerous because we don't know what the thread is doing at the time. there is only one case where we know it is paused, but that isn't here/now
    }
    else
    {
        //since we already dealt with it, we don't put it back in the ReUseQueue, but we do re-insert it to the end of the UsedQueue
        m_ReUseQueue->append(selfPointer);
    }
}
void DataBufferGenerator::sendBufferReadySignal(QObject *whoToNotify, GeneratedDataBuffer *bufferPtr, bool rushDangerous)
{
    QMetaObject::invokeMethod(whoToNotify, "hereIsYourBuffer", (rushDangerous ? Qt::DirectConnection : Qt::QueuedConnection), Q_ARG(GeneratedDataBuffer*,bufferPtr));
}

//old constructor
#if 0
    m_ReUseQueueMutex = new QMutex();
    m_UsedQueueMutex = new QMutex();

    m_ReUseQueueMutex = new QQueue<GeneratedDataBuffer*>();
    m_UsedQueue = new QQueue<GeneratedDataBuffer*>();
#if 0
}
GeneratedDataBuffer *DataBufferGenerator::giveMeADataBuffer()
{
#if 0
    QMutexLocker scopedReUseQueueLock(m_ReUseQueueMutex);
    if(m_ReUseQueue->size() > 0)
    {
        return m_ReUseQueue->dequeue();
    }
    //scopedReUseQueueLock.unlock(); //pretty sure i don't need this locked at this point, since i'm neither going to be pulling from or pushing into the ReUse Queue...
    //else, see if we've already generated too many/too much. if so, return null(?????). if we can generate more, do so.
    QMutexLocker scopedUsedQueueLock(m_UsedQueueMutex);
    //it is assumed/known at this point that ReUseQueue is 0 (i should keep it locked so i know it stays 0), so we only have what's in the UsedQueue to add up
    int sizeUsed = m_UsedQueue->size() * m_GeneratedDataBufferSize;
    if(sizeUsed >= m_TotalMaxSizeOfAllGeneratedDataBuffers)
    {
        return null; //TODO: should i be letting them be scheduling a send here and send it later when the ReUsed buffer fills up a bit? that would imply that this DataBufferGenerator is on it's own thread. currently it's thread-apathetic and can be accessed by multiple threads. i just don't know what design is better
    }
    if((m_TotalMaxSizeOfAllGeneratedDataBuffers - sizeUsed) > m_GeneratedDataBufferSize) //if we have enough free space to allocate one more buffer, do it.
    {
        GeneratedDataBuffer *newGeneratedDataBuffer = new GeneratedDataBuffer(m_GeneratedDataBufferSize);
        connect(newGeneratedDataBuffer, SIGNAL(doneUsing(GeneratedDataBuffer*)), this, SLOT(reclaim(GeneratedDataBuffer*))); //this would imply this running on it's own thread?
        //also, i might not need to send GeneratedDataBuffer in my doneUsing signal, they might be able to see who it's from just by looking at sender()

        m_UsedQueue->append(newGeneratedDataBuffer);

        //i think a re-design is in order, DataBufferGenerator needs to have a thread and it's own backend
    }

}
#endif
