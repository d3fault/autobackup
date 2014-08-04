#include "directmethodcallvsvirtualmethodcallvsinvokemethodvssignalslotactivationcli.h"

#include <QCoreApplication>

#include "directmethodcallvsvirtualmethodcallvsinvokemethodvssignalslotactivation.h"

DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli::DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation *benchmarker = new DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation(this);
    connect(benchmarker, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(benchmarker, SIGNAL(quitRequested()), qApp, SLOT(quit()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(benchmarker, "runBenchmark", Q_ARG(quint64, 5000));
}
void DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivationCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
