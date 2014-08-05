#include "directmethodcallvsvirtualmethodcallvsinvokemethodvssignalslotactivation.h"

#include <QElapsedTimer>
#include <QCryptographicHash>

#define MY_METHOD_BODY_LAWL \
m_GlobalStringLength += m_GlobalIterationCount; \
++m_GlobalIterationCount;

#if 0
QString blah = "I have a rather large penis. I mean, it isn't enormous... but it's certainly above average. It's approximately " + QString::number(m_GlobalIterationCount++) + " millimeters long. I can usually feel the back wall when I thrust."; \
m_GlobalStringLength += blah.length();
#endif

#if 0 //turns out this is so time consuming that the method calls become negligible
QByteArray blahBA = blah.toUtf8(); \
QCryptographicHash hasher(QCryptographicHash::Sha1); \
hasher.addData(blahBA); \
QByteArray resultBA = hasher.result().toHex(); \
Q_UNUSED(resultBA);
#endif

DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation(QObject *parent)
    : QObject(parent)
{ }
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doDirectMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        someShortOperationSoTheCallingIsWhatWeBenchmark();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doVirtualMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        someShortVirtualOperationSoTheCallingIsWhatWeBenchmark();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doInvokeMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        QMetaObject::invokeMethod(this, "someShortOperationSoTheCallingIsWhatWeBenchmark", Qt::DirectConnection);
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doSignalSlotActivationBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        emit someShortOperationSoTheCallingIsWhatWeBenchmarkRequested();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::someShortVirtualOperationSoTheCallingIsWhatWeBenchmark()
{
    //someShortOperationSoTheCallingIsWhatWeBenchmark(); <-- nope, because then that's two calls! using a macro instaed <3
    MY_METHOD_BODY_LAWL
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::runBenchmark(quint64 iterationsOfEach)
{
    m_GlobalIterationCount = 0;
    m_GlobalStringLength = 0;
    connect(this, SIGNAL(someShortOperationSoTheCallingIsWhatWeBenchmarkRequested()), this, SLOT(someShortOperationSoTheCallingIsWhatWeBenchmark()), Qt::DirectConnection);

    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    //Direct method call
    doDirectMethodCallBenchmark(iterationsOfEach);
    qint64 directMethodCallElapsedTime = elapsedTimer.restart();

    //Virtual method call
    doVirtualMethodCallBenchmark(iterationsOfEach);
    qint64 virtualMethodCallElapsedTime = elapsedTimer.restart();

    //Invoke method
    doInvokeMethodCallBenchmark(iterationsOfEach);
    qint64 invokeMethodElapsedTime = elapsedTimer.restart();

    //Signal
    doSignalSlotActivationBenchmark(iterationsOfEach);
    qint64 signalSlotActivationElapsedTime = elapsedTimer.restart();

    emit o("Ignore this random number lol (I just don't want it to get optimized out): " + QString::number(m_GlobalStringLength));
    emit o("");

    emit o("Direct Method Call: " + QString::number(directMethodCallElapsedTime));
    emit o("Virtual Method Call: " + QString::number(virtualMethodCallElapsedTime));
    emit o("Invoke Method: " + QString::number(invokeMethodElapsedTime));
    emit o("Signal slot activation: " + QString::number(signalSlotActivationElapsedTime));
    emit quitRequested();
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::someShortOperationSoTheCallingIsWhatWeBenchmark()
{
    MY_METHOD_BODY_LAWL
}
