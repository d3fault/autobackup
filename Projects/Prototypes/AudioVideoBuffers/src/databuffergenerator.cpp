#include "databuffergenerator.h"

DataBufferGenerator::DataBufferGenerator(int totalMaxSizeOfAllGeneratedDataBuffers, int generatedDataBufferSize)
    : m_TotalMaxSizeOfAllGeneratedDataBuffers(totalMaxSizeOfAllGeneratedDataBuffers), m_GeneratedDataBufferSize(generatedDataBufferSize)
{
    m_ReUseQueueMutex = new QMutex();
    m_UsedQueueMutex = new QMutex();

    m_ReUseQueueMutex = new QQueue<GeneratedDataBuffer*>();
    m_UsedQueue = new QQueue<GeneratedDataBuffer*>();
}
GeneratedDataBuffer *DataBufferGenerator::giveMeADataBuffer()
{
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
