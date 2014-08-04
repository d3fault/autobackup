#ifndef DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATION_H
#define DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATION_H

#include <QObject>

class DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation : public QObject
{
    Q_OBJECT
public:
    explicit DirectMethodCallVsVirtualMethodCallVsInvokeMethodVsSignalSlotActivation(QObject *parent = 0);
private:
    quint64 m_GlobalIterationCount;

    void doDirectMethodCallBenchmark(qint64 iterations);
    void doVirtualMethodCallBenchmark(qint64 iterations);
    void doInvokeMethodCallBenchmark(qint64 iterations);
    void doSignalSlotActivationBenchmark(qint64 iterations);

    virtual void someMildlyTimeConsumingVirtualOperation();
signals:
    void o(const QString &msg);
    void someMildlyTimeConsumingOperationRequested();
    void quitRequested();
public slots:
    void runBenchmark(quint64 iterationsOfEach);
private slots:
    void someMildlyTimeConsumingOperation();
};

#endif // DIRECTMETHODCALLVSVIRTUALMETHODCALLVSINVOKEMETHODVSSIGNALSLOTACTIVATION_H
