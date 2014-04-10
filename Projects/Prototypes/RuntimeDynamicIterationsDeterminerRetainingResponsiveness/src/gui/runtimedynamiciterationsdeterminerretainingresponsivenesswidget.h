#ifndef RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSWIDGET_H
#define RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QElapsedTimer>

class RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget : public QWidget
{
    Q_OBJECT
public:
    RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget(QWidget *parent = 0);
private:
    QLineEdit *m_MSecTargetMaxResponseTimeLineEdit;
    QPlainTextEdit *m_Debug;
    QElapsedTimer m_PingPongBenchmarkTimer;
signals:
    void determineIterationsToRetainResponsivenessWithinRequested(qint64 msecTargetMaxResponseTime);
    void startDoingWorkUsingDeterminedIterationsRequested();
    void pingRequested();
    void stopDoingWorkUsingDeterminedIterationsRequested();
public slots:
    void handleD(const QString &msg);
    void handlePong();
private slots:
    void handleDetermineIterationsButtonClicked();
    void handlePingButtonClicked();
};

#endif // RUNTIMEDYNAMICITERATIONSDETERMINERRETAININGRESPONSIVENESSWIDGET_H
