#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <QObject>
#include <QThread>
#include <QTime>

#include "Tests/signalcommunicatingheaprecyclingdatagenerator.h"
#include "Tests/signalcommunicatingheaprecyclingdataprocessor.h"

#include "Tests/mutexusingheaprecyclingdatagenerator.h"
#include "Tests/mutexusingheaprecyclingdataprocessor.h"

#include "Tests/signalcommunicatingimplicitlysharedstackdatagenerator.h"
#include "Tests/signalcommunicatingimplicitlysharedstackdataprocessor.h"

class TestDriver : public QObject
{
    Q_OBJECT
public:
    explicit TestDriver(QObject *parent = 0);
    ~TestDriver();
private:
    QTime m_Time;
    int m_Elapsed0, m_Elapsed1, m_Elapsed2;

    QThread *m_GeneratorThread;
    QThread *m_ProcessorThread;

    //TODO: stack alloc/delloc (dgaf) vs. heap recycling ____ WITHOUT thread separation (Actions). There would never be a reason/case to use a mutex for that test

    SignalCommunicatingHeapRecyclingDataGenerator *m_SignalCommunicatingHeapRecyclingDataGenerator; //'owner' of recycle list
    SignalCommunicatingHeapRecyclingDataProcessor *m_SignalCommunicatingHeapRecyclingDataProcessor; //emits back to owner

    MutexUsingHeapRecyclingDataGenerator *m_MutexUsingHeapRecyclingDataGenerator; //neither thread 'owns' the recycle list, they both lock a mutex to access it
    MutexUsingHeapRecyclingDataProcessor *m_MutexUsingHeapRecyclingDataProcessor;

    SignalCommunicatingImplicitlySharedStackDataGenerator *m_SignalCommunicatingImplicitlySharedStackDataGenerator; //there is no recycle list, but the instantiated type is a) on the stack and b) implicitly shared (implicitly shared so we can emit it across a thread to the processor)
    SignalCommunicatingImplicitlySharedStackDataProcessor *m_SignalCommunicatingImplicitlySharedStackDataProcessor; //after we 'process' it, we let it go out of scope and dgaf it


    //TODO: size of instantiated message won't matter for recycling, since we're only passing around pointer to it. But with the implicitly shared stack one it *MIGHT*.... although stack allocs are hella fast since all you do is move the stack pointer (but wait... the d pointer does a new on the heap for whatever size... so YES, specifying the object size _DOES_ matter for the stack examples. Find the crossover point where it's cheaper to use either mutex or recycling rather than stack allocating)

signals:
    void d(const QString &);
    void initialized();
public slots:
    void init();
    void runTests();

    void handleSignalHeapRecycleTestDone(int numGenerated,int totalSize);
    void handleMutexHeapRecycleTestDone(int numGenerated,int totalSize);
    void handleSignalStackTestDone();
};

#endif // TESTDRIVER_H
