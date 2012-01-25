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
    QQueue<GeneratedDataBuffer*> *m_WriteCacheEmptyBufferQueue;
    GeneratedDataBuffer *m_CurrentEmptyDataBuffer;
    DataBufferGenerator *m_DataBufferGenerator; //for write cache only, not the memory cache
    QMutex *m_WriteCacheEmptyBufferMutex;
    void privateAppend(void *dataPtr, int dataSize);
    volatile bool m_WeHaveADataBuffer;
    volatile bool m_AppendIsWaiting;
signals:

public slots:
    void handleBufferDelivery(GeneratedDataBuffer *newBuffer);
};

#endif // LOCALFILE_H
