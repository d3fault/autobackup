#ifndef DATABUFFERGENERATOR_H
#define DATABUFFERGENERATOR_H

#include <QObject>
#include <QMutexLocker>
#include <QMutex>
#include <QQueue>

#include <generateddatabuffer.h>

//we allocate the GeneratedDataBuffers, connect to and watch their "finishedUsingBuffer" signal, in which case it puts them in a queue to be re-used instead of allocating new ones. there is a specifiable maximum size of all buffers combined and of the individual buffers themselves
//this class should be a singleton, but could be carefully managed with only 1 instance per app. sameshit tho, fuck semantics. singleton is easier. the only
//actually no, don't use a singleton. we need to be able to specify the size of the buffers (children) on a per-DataBufferGenerator (parent) basis
//the only way [needed] that DataBufferGenerator is thread safe is by means of a Mutex guarding the queue(s) of re-used (and checked-out) GeneratedDataBuffers
class DataBufferGenerator : public QObject
{
    Q_OBJECT
public:
    DataBufferGenerator(int totalMaxSizeOfAllGeneratedDataBuffers, int generatedDataBufferSize);
    GeneratedDataBuffer *giveMeADataBuffer(int bufferSize);
private:
    int m_TotalMaxSizeOfAllGeneratedDataBuffers;
    int m_GeneratedDataBufferSize;
    QMutex *m_ReUseQueueMutex;
    QMutex *m_UsedQueueMutex;
    QQueue<GeneratedDataBuffer*> *m_ReUseQueue;
    QQueue<GeneratedDataBuffer*> *m_UsedQueue;

    volatile bool m_ScheduleWhenReUseSubmitted;
    QObject *m_LastToAskForABuffer;
    void sendBufferReadySignal(QObject *whoToNotify, GeneratedDataBuffer *bufferPtr, bool rushDangerous = false);
signals:
    void doneUsing(GeneratedDataBuffer *selfPointer);
public slots:
    void scheduleBufferDelivery(QObject objectToDeliverTo, bool rushDangerous = false);
private slots:
    void reclaim(GeneratedDataBuffer *selfPointer);
};

#endif // DATABUFFERGENERATOR_H
