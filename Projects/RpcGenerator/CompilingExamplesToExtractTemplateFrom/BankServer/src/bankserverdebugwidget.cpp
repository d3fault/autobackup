#include "bankserverdebugwidget.h"

bankServerDebugWidget::bankServerDebugWidget(QWidget *parent)
    : QWidget(parent)
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
    connect(m_SimulatePendingBalanceDetectedBroadcastButton, SIGNAL(clicked()), this, SIGNAL(simulatePendingBalanceDetectedBroadcastRequested()));
    connect(m_SimulateConfirmedBalanceDetectedBroadcastButton, SIGNAL(clicked()), this, SIGNAL(simulateConfirmedBalanceDetectedBroadcastRequested()));
    connect(m_StopBusinessButton, SIGNAL(clicked()), this, SIGNAL(stopBusinessRequested()));

    setGuiEnabled(false); //We wait for initialized
}
void bankServerDebugWidget::setGuiEnabled(bool enabled)
{
    m_SimulatePendingBalanceDetectedBroadcastButton->setEnabled(enabled);
    m_SimulateConfirmedBalanceDetectedBroadcastButton->setEnabled(enabled);
}
void bankServerDebugWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void bankServerDebugWidget::businessInitialized()
{
    setGuiEnabled(true);
}
