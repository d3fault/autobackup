#include "bankserverdebugwidget.h"

bankServerDebugWidget::bankServerDebugWidget(BankServerTest *bankServerTest, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(bankServerTest, parent)
{ }
void bankServerDebugWidget::addButtonsToLayoutAndConnectToBackend()
{
    m_SimulatePendingBalanceDetectedBroadcast = new QPushButton("Simulate Pending Balance Detected");
    m_SimulateConfirmedBalanceDetectedBroadcast = new QPushButton("Simulate Confirmed Balance Detected");

    m_Layout->addWidget(m_SimulatePendingBalanceDetectedBroadcast);
    m_Layout->addWidget(m_SimulateConfirmedBalanceDetectedBroadcast);

    connect(m_SimulatePendingBalanceDetectedBroadcast, SIGNAL(clicked()), static_cast<BankServerTest*>(m_DebuggableStartableStoppableBackend), SLOT(simulatePendingBalanceDetectedBroadcast()));
    connect(m_SimulateConfirmedBalanceDetectedBroadcast, SIGNAL(clicked()), static_cast<BankServerTest*>(m_DebuggableStartableStoppableBackend), SLOT(simulateConfirmedBalanceDetectedBroadcast()));
}
bankServerDebugWidget::~bankServerDebugWidget()
{
}
