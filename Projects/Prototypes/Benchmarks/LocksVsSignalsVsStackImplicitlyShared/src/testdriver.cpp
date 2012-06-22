#include "testdriver.h"

TestDriver::TestDriver(QObject *parent) :
    QObject(parent)
{

}
void TestDriver::init()
{
    m_GeneratorThread = new QThread();
    m_ProcessorThread = new QThread();

    QString sampleData("wpqeiaiqmnaewar"); //the 'generator' replaces all the A's with B's, and the 'processor' does the opposite.

    int loopCount = 1000; //do the above process of replacing characters in a string this many times

    int sizeMultiplier = 20; //repeat sampleData this many times for the actual operations


    m_SignalCommunicatingHeapRecyclingDataGenerator = new SignalCommunicatingHeapRecyclingDataGenerator(loopCount, sizeMultiplier, sampleData);
    m_SignalCommunicatingHeapRecyclingDataProcessor = new SignalCommunicatingHeapRecyclingDataProcessor();

    m_MutexUsingHeapRecyclingDataGenerator = new MutexUsingHeapRecyclingDataGenerator(loopCount, sizeMultiplier, sampleData);
    m_MutexUsingHeapRecyclingDataProcessor = new MutexUsingHeapRecyclingDataProcessor(m_MutexUsingHeapRecyclingDataGenerator);

    m_SignalCommunicatingImplicitlySharedStackDataGenerator = new SignalCommunicatingImplicitlySharedStackDataGenerator(loopCount, sizeMultiplier, sampleData);
    m_SignalCommunicatingImplicitlySharedStackDataProcessor = new SignalCommunicatingImplicitlySharedStackDataProcessor(loopCount);

    m_SignalCommunicatingHeapRecyclingDataGenerator->moveToThread(m_GeneratorThread);
    m_MutexUsingHeapRecyclingDataGenerator->moveToThread(m_GeneratorThread);
    m_SignalCommunicatingImplicitlySharedStackDataGenerator->moveToThread(m_GeneratorThread);

    m_SignalCommunicatingHeapRecyclingDataProcessor->moveToThread(m_ProcessorThread);
    m_MutexUsingHeapRecyclingDataProcessor->moveToThread(m_ProcessorThread);
    m_SignalCommunicatingImplicitlySharedStackDataProcessor->moveToThread(m_ProcessorThread);

    m_GeneratorThread->start();
    m_ProcessorThread->start();

    connect(this, SIGNAL(initialized()), m_SignalCommunicatingHeapRecyclingDataGenerator, SLOT(init()));
    connect(this, SIGNAL(initialized()), m_SignalCommunicatingHeapRecyclingDataProcessor, SLOT(init()));

    connect(this, SIGNAL(initialized()), m_MutexUsingHeapRecyclingDataGenerator, SLOT(init()));
    connect(this, SIGNAL(initialized()), m_MutexUsingHeapRecyclingDataProcessor, SLOT(init()));

    connect(this, SIGNAL(initialized()), m_SignalCommunicatingImplicitlySharedStackDataGenerator, SLOT(init()));
    connect(this, SIGNAL(initialized()), m_SignalCommunicatingImplicitlySharedStackDataProcessor, SLOT(init()));

    connect(m_SignalCommunicatingHeapRecyclingDataGenerator, SIGNAL(testFinished(int,int)), this, SLOT(handleSignalHeapRecycleTestDone(int,int)));
    connect(m_MutexUsingHeapRecyclingDataGenerator, SIGNAL(testFinished(int,int)), this, SLOT(handleMutexHeapRecycleTestDone(int,int)));
    //connect(m_SignalCommunicatingImplicitlySharedStackDataGenerator, SIGNAL(testFinished()), this, SLOT(handleSignalStackTestDone()));
    //for the stack test, since there is no recycling the processor has to detect and emit the finished
    connect(m_SignalCommunicatingImplicitlySharedStackDataProcessor, SIGNAL(testFinished()), this, SLOT(handleSignalStackTestDone()));

    emit initialized();
}
void TestDriver::runTests()
{
    //test specific connections
    connect(m_SignalCommunicatingHeapRecyclingDataGenerator, SIGNAL(bytesGenerated(QString*)), m_SignalCommunicatingHeapRecyclingDataProcessor, SLOT(processGeneratedData(QString*)));
    connect(m_SignalCommunicatingHeapRecyclingDataProcessor, SIGNAL(doneWithData(QString*)), m_SignalCommunicatingHeapRecyclingDataGenerator, SLOT(recycledUsed(QString*)));

    //connect(m_SignalCommunicatingHeapRecyclingDataGenerator, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    m_Time.start();

    QMetaObject::invokeMethod(m_SignalCommunicatingHeapRecyclingDataGenerator, "startTest", Qt::QueuedConnection);
}
void TestDriver::handleSignalHeapRecycleTestDone(int numGenerated, int totalSize)
{
    m_Elapsed0 = m_Time.elapsed();

    m_SignalCommunicatingHeapRecyclingDataGenerator->cleanup();

    QString blah(QString("Signal Communicating Heap Recycling Took: ") + QString::number(m_Elapsed0) + QString(" ms -- ") + QString::number(numGenerated) + QString(" objects were generated, totalling: ") + QString::number(totalSize) + QString(" characters"));
    emit d(blah);


    //mutex test related connections
    connect(m_MutexUsingHeapRecyclingDataGenerator, SIGNAL(bytesGenerated(QString*)), m_MutexUsingHeapRecyclingDataProcessor, SLOT(processGeneratedData(QString*)));
    //he does not emit it back like the first test, it is placed back safely using a mutex

    m_Time.restart();

    QMetaObject::invokeMethod(m_MutexUsingHeapRecyclingDataGenerator, "startTest", Qt::QueuedConnection);
}
void TestDriver::handleMutexHeapRecycleTestDone(int numGenerated, int totalSize)
{
    m_Elapsed1 = m_Time.elapsed();

    m_MutexUsingHeapRecyclingDataGenerator->cleanup();

    emit d(QString("Half Signal Communicating / Mutex Locking Returning Heap Recycling Took: ") + QString::number(m_Elapsed1) + QString(" ms -- ") + QString::number(numGenerated) + QString(" objects were generated, totalling: ") + QString::number(totalSize) + QString(" characters"));


    //stack test related connections
    connect(m_SignalCommunicatingImplicitlySharedStackDataGenerator, SIGNAL(bytesGenerated(QString)), m_SignalCommunicatingImplicitlySharedStackDataProcessor, SLOT(processGeneratedData(QString)));
    //he does not emit it back, nor does he even return it. he lets it go out of scope and dgafs it

    m_Time.restart();

    QMetaObject::invokeMethod(m_SignalCommunicatingImplicitlySharedStackDataGenerator, "startTest", Qt::QueuedConnection);
}
void TestDriver::handleSignalStackTestDone()
{
    m_Elapsed2 = m_Time.elapsed();

    emit d(QString("Signal Communicating Implicitly Shared Stack De/Allocating Took: ") + QString::number(m_Elapsed2) + QString(" ms"));
}
TestDriver::~TestDriver()
{
    delete m_SignalCommunicatingImplicitlySharedStackDataProcessor;
    delete m_SignalCommunicatingImplicitlySharedStackDataGenerator;

    delete m_MutexUsingHeapRecyclingDataProcessor;
    delete m_MutexUsingHeapRecyclingDataGenerator;

    delete m_SignalCommunicatingHeapRecyclingDataProcessor;
    delete m_SignalCommunicatingHeapRecyclingDataGenerator;

    if(m_ProcessorThread->isRunning())
    {
        m_ProcessorThread->quit();
        m_ProcessorThread->wait();
    }
    delete m_ProcessorThread;
    if(m_GeneratorThread->isRunning())
    {
        m_GeneratorThread->quit();
        m_GeneratorThread->wait();
    }
    delete m_GeneratorThread;
}
