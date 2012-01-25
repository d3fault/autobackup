#include "localfile.h"

LocalFile::LocalFile(int memoryCacheSizeInBytes, DataBufferGenerator *dataBufferGenerator)
    : m_MemoryCacheSize(memoryCacheSizeInBytes)
{
    m_MemoryCache = new QSharedMemory(MEMORY_KEY);
    //m_WriteCache = new QSharedMemory(WRITE_CACHE_KEY);

    m_MemoryCache->create(memoryCacheSizeInBytes);
    //m_WriteCache->create(WRITE_CACHE_SIZE);
    m_DataBufferGenerator = dataBufferGenerator;

    m_WriteCacheEmptyBufferQueue = new QQueue<GeneratedDataBuffer*>();
    m_WriteCacheEmptyBufferMutex = new QMutex();

    QMetaObject::invokeMethod(m_DataBufferGenerator, "scheduleBufferDelivery", Qt::QueuedConnection, Q_ARG(QObject, this));
}
void LocalFile::append(void *newData, int dataSize)
{
    //todo: calculate bytesFreeInMemoryCache
#if 0
    if(bytesFreeInMemoryCache > 0)
    {
        if()
    }
#endif
    //put in memory if we need the data, but always:
    //schedule to be saved to hdd
    //lock the writeQueue
    //write to the writeQueue
    //???schedule an event to ourself to actually do the writing???
#if 0
    if(!m_WriteCache->lock())
    {
        //deal with failure, if create returns false for example
    }
    memcpy(m_WriteCache->data()+writeCacheOffset, newData, dataSize);
    m_WriteCache->unlock();
#endif
    //what if writeCache is full?????????
    //this is a scenario where i'd like to use DataBufferGenerator. i could just allocate some more memory if some is available, and at worst case scenario just block/flush the write cache right here until we have some buffers available to copy into
    //DataBufferGenerator needs to work on the assumption that we won't always use everything we've read, and we don't always get what we ask for. in terms of size of buffer returned. the underlying buffer stays a constant size. it's the maximum. but the databuffer should have a member/property that tells us how much that contained data really is. it should encapsulate streaming. or no it shouldn't. circular buffer with simulated 'streaming' or just buffer packets and lots of them?

    //ok so for the in memory, i can use a QSharedData
    //but for the WriteCache, i should use the DataBufferGenerator
    //this way, i use as much memory as i need while still being as efficient as possible with my already-allocated memory. it's likely it will use like 3 or so (one ready to be written to with the curl data, one already written to that needs to be written to disk, one waiting to be re-used because it's content was already written to disk)... but can scale up to as many as it needs.

    //but another problem, isn't the buffer generator also on it's own thread? i mean, not quite yet... but haven't i redesigned the usage in the past 20 minutes so it is on it's own thread? i guess i could try as hard as possible to always have at least 1 buffer ready to be written to for the write cache, whether needing to allocate or not. but we do this at the end of each operation, not NOW. NOW is too late, we are holding the curl thread up, which we don't want to do
#if 0
    if(weHaveADataBuffer)
    {
        memcpy(m_CurrentDataBuffer->data(), newData, dataSize);
        m_CurrentDataBuffer->setSize(dataSize);
    }
#endif

    QMutexLocker scopedLock(m_WriteCacheEmptyBufferMutex);
    privateAppend(newData, dataSize);
}
void LocalFile::handleBufferDelivery(GeneratedDataBuffer *newBuffer)
{
    QMutexLocker scopedLock(m_WriteCacheEmptyBufferMutex);
    m_WriteCacheEmptyBufferQueue->append(newBuffer);
    if(m_WriteCacheEmptyBufferQueue->size() > 0)
    {
        m_CurrentEmptyDataBuffer = m_WriteCacheEmptyBufferQueue->first();
        //ok i think i get why my brain is fucking up
        //the queue is not necessary. we should just always at one time have one m_CurrentEmptyDataBuffer; once it's full, we append it to a queue that is processed by this thread separately that actually does the writing
    }
    weHaveADataBuffer = true;
}
void LocalFile::privateAppend(void *dataPtr, int dataSize)
{
    int dataLeftToWrite = dataSize;
    int lastSizeUsed, lastTotalCapacity;
    while(dataLeftToWrite > 0)
    {
        lastSizeUsed = m_WriteCacheEmptyBufferQueue->last()->getSizeUsed();
        lastTotalCapacity = m_WriteCacheEmptyBufferQueue->last()->getCapacity();
        if(lastSizeUsed < lastTotalCapacity)
        {
            //append to the last data buffer until it is full
            int sizeToAppend = qMin((lastTotalCapacity-lastSizeUsed), dataSize);
            memcpy(m_WriteCacheEmptyBufferQueue->last()->data()+lastSizeUsed, dataPtr, sizeToAppend);
            dataPtr += sizeToAppend;
            dataSize -= sizeToAppend;
            dataLeftToWrite -= sizeToAppend;
        }
        else //last data buffer full, use a new one?
        {

        }
    }
}
