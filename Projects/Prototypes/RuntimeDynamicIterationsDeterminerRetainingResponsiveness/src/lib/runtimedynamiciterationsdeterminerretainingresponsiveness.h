#ifndef RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESS_H
#define RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESS_H

#include <QObject>
#include <QElapsedTimer>

class RuntimeDynamicIterationsDeterminerRetainingResponsiveness : public QObject
{
    Q_OBJECT
public:
    explicit RuntimeDynamicIterationsDeterminerRetainingResponsiveness(QObject *parent = 0);
private:
    qint64 m_DeterminedIterationsToRetainResponsiveness;
    bool m_StopRequested;

    //determination stage
    QElapsedTimer m_BenchmarkTimer;

    void doSingleIterationOfWork();
signals:
    void d(const QString &);
    void pong();
public slots:
    void determineIterationsToRetainResponsivenessWithin(qint64 msecTargetMaxResponseTime);
    void startDoingWorkUtilizingDeterminedNumberOfIterations();
    void doMultipleUnitsOfWorkUsingDeterminedIterations();
    void ping();
    void stopDoingWork();
};

#endif // RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESS_H
