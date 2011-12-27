#ifndef THREADSAFEQUEUEBYMUTEX_H
#define THREADSAFEQUEUEBYMUTEX_H

#include <QObject>

#include <QByteArray>
#include <QMutex>

class ThreadSafeQueueByMutex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadSafeQueueByMutex(QObject *parent = 0);
private:
    QByteArray *m_ByteArray;
    QMutex *m_Mutex;
public:
    void append(const QByteArray &toAppend, int size);
    QByteArray deQueue(int howMuchToDeQueue);
};

#endif // THREADSAFEQUEUEBYMUTEX_H
