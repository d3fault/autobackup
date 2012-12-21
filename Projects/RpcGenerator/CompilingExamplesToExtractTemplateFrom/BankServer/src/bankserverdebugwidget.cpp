#include "bankserverdebugwidget.h"

bankServerDebugWidget::bankServerDebugWidget(QWidget *parent)
    : QWidget(parent), m_BusinessStarted(false)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();

    m_StartBusinessButton = new QPushButton("Start");
    m_SimulatePendingBalanceDetectedBroadcastButton = new QPushButton("Simulate Pending Balance Detected");
    m_SimulateConfirmedBalanceDetectedBroadcastButton = new QPushButton("Simulate Confirmed Balance Detected");
    m_StopBusinessButton = new QPushButton("Stop");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartBusinessButton);
    m_Layout->addWidget(m_SimulatePendingBalanceDetectedBroadcastButton);
    m_Layout->addWidget(m_SimulateConfirmedBalanceDetectedBroadcastButton);
    m_Layout->addWidget(m_StopBusinessButton);

    connect(m_StartBusinessButton, SIGNAL(clicked()), this, SIGNAL(startBusinessRequested()));
    connect(m_StartBusinessButton, SIGNAL(clicked()), this, SLOT(disableUserInteraction()));

    //no need to wait for these user interactions to complete (disabling the gui) because the design of the protocol shit is inherently asynchronous and it would be stupid if it weren't. starting and stopping is another matter. i SHOULD be able to spam click these and they should work perfectly fine
    connect(m_SimulatePendingBalanceDetectedBroadcastButton, SIGNAL(clicked()), this, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()));
    connect(m_SimulateConfirmedBalanceDetectedBroadcastButton, SIGNAL(clicked()), this, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()));

    connect(m_StopBusinessButton, SIGNAL(clicked()), this, SIGNAL(stopBusinessRequested()));
    connect(m_StopBusinessButton, SIGNAL(clicked()), this, SLOT(disableUserInteraction()));

    disableUserInteraction(); //We wait for initialized
}
void bankServerDebugWidget::setUserInteractionEnabled(bool enabled)
{
    m_StartBusinessButton->setEnabled(enabled);
    m_SimulatePendingBalanceDetectedBroadcastButton->setEnabled(enabled && m_BusinessStarted);
    m_SimulateConfirmedBalanceDetectedBroadcastButton->setEnabled(enabled && m_BusinessStarted);
    m_StopBusinessButton->setEnabled(enabled);
}
void bankServerDebugWidget::handleD(const QString &msg)
{
    enableUserInteraction();
    m_Debug->appendPlainText(msg);
}
void bankServerDebugWidget::handleBusinessInitialized()
{
    handleD("gui noticed business has initialized");
}
void bankServerDebugWidget::handleBusinessStarted()
{
    m_BusinessStarted = true;
    enableUserInteraction();
    handleD("gui noticed business has started");
}
void bankServerDebugWidget::handleBusinessStopped()
{
    m_BusinessStarted = false;
    enableUserInteraction();
    handleD("gui noticed business has stopped");
}
