#include "threadsafequeuebymutex.h"

ThreadSafeQueueByMutex::ThreadSafeQueueByMutex(QObject *parent) :
    QObject(parent)
{
    m_ByteArray = new QByteArray();
    m_Mutex = new QMutex();
}
void ThreadSafeQueueByMutex::append(const QByteArray &toAppend, int size)
{
    m_Mutex->lock();

    m_ByteArray->append((const char*)toAppend.data(),size);

    m_Mutex->unlock();
}
QByteArray ThreadSafeQueueByMutex::deQueue(int howMuchToDeQueue)
{
    m_Mutex->lock();

    QByteArray ret = m_ByteArray->left(howMuchToDeQueue);
    m_ByteArray->remove(0, ret.size());

    m_Mutex->unlock();

    return ret;
}
