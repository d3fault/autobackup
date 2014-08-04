#include "directmethodcallvsvirtualmethodcallvsinvokemethodvssignalslotactivation.h"

#include <QElapsedTimer>
#include <QCryptographicHash>

#define MY_METHOD_BODY_LAWL \
QString blah = "I have a rather large penis. I mean, it isn't enormous... but it's certainly above average. It's approximately " + QString::number(m_GlobalIterationCount++) + " millimeters long. I can usually feel the back wall when I thrust."; \
QByteArray blahBA = blah.toUtf8(); \
QCryptographicHash hasher(QCryptographicHash::Sha1); \
hasher.addData(blahBA); \
QByteArray resultBA = hasher.result().toHex(); \
Q_UNUSED(resultBA);

DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation(QObject *parent)
    : QObject(parent)
{ }
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doDirectMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        someMildlyTimeConsumingOperation();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doVirtualMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        someMildlyTimeConsumingVirtualOperation();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doInvokeMethodCallBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        QMetaObject::invokeMethod(this, "someMildlyTimeConsumingOperation", Qt::DirectConnection);
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::doSignalSlotActivationBenchmark(qint64 iterations)
{
    for(int i = 0; i < iterations; ++i)
    {
        emit someMildlyTimeConsumingOperationRequested();
    }
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::someMildlyTimeConsumingVirtualOperation()
{
    //someMildlyTimeConsumingOperation(); <-- nope, because then that's two calls! using a macro instaed <3
    MY_METHOD_BODY_LAWL
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::runBenchmark(quint64 iterationsOfEach)
{
    m_GlobalIterationCount = 0;
    connect(this, SIGNAL(someMildlyTimeConsumingOperationRequested()), this, SLOT(someMildlyTimeConsumingOperation()), Qt::DirectConnection);

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

    emit o("Direct Method Call: " + QString::number(directMethodCallElapsedTime));
    emit o("Virtual Method Call: " + QString::number(virtualMethodCallElapsedTime));
    emit o("Invoke Method: " + QString::number(invokeMethodElapsedTime));
    emit o("Signal slot activation: " + QString::number(signalSlotActivationElapsedTime));
    emit quitRequested();
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation::someMildlyTimeConsumingOperation()
{
    MY_METHOD_BODY_LAWL
}
