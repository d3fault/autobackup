#include "runtimedynamiciterationsdeterminerretainingresponsivenesswidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget::RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    QHBoxLayout *msecTargetMaxResponseTimeRow = new QHBoxLayout();
    QLabel *msecTargetMaxResponseTimeLabel = new QLabel(tr("Target milliseconds response time (soft max):"));
    m_MSecTargetMaxResponseTimeLineEdit = new QLineEdit("250");
    msecTargetMaxResponseTimeRow->addWidget(msecTargetMaxResponseTimeLabel);
    msecTargetMaxResponseTimeRow->addWidget(m_MSecTargetMaxResponseTimeLineEdit);
    myLayout->addLayout(msecTargetMaxResponseTimeRow);
    QPushButton *determineIterationsButton = new QPushButton(tr("Determine number of iterations that can be performed under that response time"));
    myLayout->addWidget(determineIterationsButton);
    QPushButton *startDoingWorkUsingDeterminedIterationsButton = new QPushButton(tr("Start doing work using determined number of iterations between 'message processing'"));
    myLayout->addWidget(startDoingWorkUsingDeterminedIterationsButton);
    QPushButton *pingBackendButton = new QPushButton(tr("Ping the backend (timing the time it takes to response)"));
    myLayout->addWidget(pingBackendButton);
    QPushButton *stopDoingWorkButton = new QPushButton(tr("Stop doing work"));
    myLayout->addWidget(stopDoingWorkButton);
    m_Debug = new QPlainTextEdit();
    myLayout->addWidget(m_Debug);
    connect(determineIterationsButton, SIGNAL(clicked()), this, SLOT(handleDetermineIterationsButtonClicked()));
    connect(startDoingWorkUsingDeterminedIterationsButton, SIGNAL(clicked()), this, SIGNAL(startDoingWorkUsingDeterminedIterationsRequested()));
    connect(pingBackendButton, SIGNAL(clicked()), this, SLOT(handlePingButtonClicked()));
    connect(stopDoingWorkButton, SIGNAL(clicked()), this, SIGNAL(stopDoingWorkUsingDeterminedIterationsRequested()));
    setLayout(myLayout);
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget::handlePong()
{
    qint64 pingPongElapsedTime = m_PingPongBenchmarkTimer.elapsed();
    handleD("Ping -> Pong took " + QString::number(pingPongElapsedTime) + " milliseconds. Is that not more than twice what's in the above line edit :-D?");
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget::handleDetermineIterationsButtonClicked()
{
    bool convertOk = false;
    qint64 msecTargetMaxResponseTime = static_cast<qint64>(m_MSecTargetMaxResponseTimeLineEdit->text().toInt(&convertOk));
    if(!convertOk)
    {
        handleD(tr("enter a valid number for target response time"));
        return;
    }
    emit determineIterationsToRetainResponsivenessWithinRequested(msecTargetMaxResponseTime);
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessWidget::handlePingButtonClicked()
{
    m_PingPongBenchmarkTimer.start();
    emit pingRequested();
}
