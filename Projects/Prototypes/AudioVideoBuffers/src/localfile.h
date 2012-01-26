#ifndef LOCALFILE_H
#define LOCALFILE_H

#include <QObject>
#include <QSharedMemory>
#include <QQueue>
#include <QMutexLocker>
#include <QMutex>

#include <databuffergenerator.h>
#include <generateddatabuffer.h>

#define MEMORY_KEY "localfilememorykeyqt"
#define WRITE_CACHE_KEY "writecachekeyqt"

#define WRITE_CACHE_SIZE ((1024 * 1024) * 4) //4mb

class LocalFile : public QObject
{
    Q_OBJECT
public:
    explicit LocalFile(int memoryCacheSizeInBytes, DataBufferGenerator *dataBufferGenerator);
    void append(const char *newData);
private:
    int m_MemoryCacheSize;
    QSharedMemory *m_MemoryCache;
    //QSharedMemory *m_WriteCache;
    QQueue<GeneratedDataBuffer*> *m_WriteCacheFilledToBeWrittenToDiskBufferQueue;
    GeneratedDataBuffer *m_CurrentEmptyDataBuffer;
    DataBufferGenerator *m_DataBufferGenerator; //for write cache only, not the memory cache
    QMutex *m_WriteCacheFilledToBeWrittenToDiskBufferMutex;
    void privateAppend(void *dataPtr, int dataSize);
    volatile bool m_WeHaveADataBuffer;
    volatile bool m_AppendIsWaiting;
    volatile bool m_WeCareWhenANewBufferArrives;
    GeneratedDataBuffer *getBufferSafeOnDifferentThreadFast(int minimumReturnSize);
    void scheduleOneBufferDelivery(bool blockingQueued = false);
    void setFileContents(int index, void *srcPtr, int srcSize);
    int m_CurrentLogicalWriteIndex; //this write index is keeping track of how much data CURL has given to us, not how much we've written to disk
    //we might use a m_CurrentActualWriteIndex to keep how much we've written to hdd
    void getNextBuffer(int bufferSize);
signals:

public slots:
    void hereIsYourBuffer(GeneratedDataBuffer *thebuffer);
    //void handleBufferDelivery(GeneratedDataBuffer *newBuffer);
};

#endif // LOCALFILE_H
