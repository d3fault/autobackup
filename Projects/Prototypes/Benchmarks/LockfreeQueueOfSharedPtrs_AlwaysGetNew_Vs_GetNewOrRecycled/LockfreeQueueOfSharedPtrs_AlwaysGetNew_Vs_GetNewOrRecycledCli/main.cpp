#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QDebug>

#define LOCKFREEQUEUEOFSHAREDPTRS_USE_ADDITIONAL_QUEUE_FOR_GET_NEW_OR_RECYCLED_HEAP_COPY 1 //this benchmark is to be run twice. once with this defined, once with it not
#include "lockfreequeueofsharedptrs.h"

const int iterations = 10000000;
const int numProducersThreads = 4;
const int numConsumerThreads = 4;

LockfreeQueueOfSharedPtrs<int> queue(0);
boost::atomic<bool> done(false);

void producer()
{
    for(int i = 0; i < iterations; ++i)
    {
        boost::shared_ptr<int> anInt(new int(i));
        while(!queue.push(anInt)) ;
    }
}
void consumer()
{
    boost::shared_ptr<int> anInt;
    while(!done)
    {
        while(queue.pop(anInt))
        {
            anInt.reset();
        }
    }
    while(queue.pop(anInt))
    {
        anInt.reset();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    boost::thread_group producerThreads, consumerThreads;
    QElapsedTimer timer;
    timer.start();

    for(int i = 0; i < numProducersThreads; ++i)
    {
        producerThreads.create_thread(producer);
    }
    for(int i = 0; i < numConsumerThreads; ++i)
    {
        consumerThreads.create_thread(consumer);
    }

    producerThreads.join_all();
    done = true;
    consumerThreads.join_all();

    qDebug() << timer.elapsed() << "milliseconds";

    return 0;
}
