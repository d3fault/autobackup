#include "bankclientdebugwidget.h"

BankClientDebugWidget::BankClientDebugWidget(BankServerClientTest *bankClientTest, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(bankClientTest, parent)
{ }
void BankClientDebugWidget::addButtonsToLayoutAndConnectToBackend()
{
    m_SimulateCreateBankAccountButton = new QPushButton("Simulate Create Bank Account Action");
    m_SimulateGetAddFundsKeyButton = new QPushButton("Simulate Get Add Funds Key Action");

    m_Layout->addWidget(m_SimulateCreateBankAccountButton);
    m_Layout->addWidget(m_SimulateGetAddFundsKeyButton);

    connect(m_SimulateCreateBankAccountButton, SIGNAL(clicked()), static_cast<BankServerClientTest*>(m_DebuggableStartableStoppableBackend), SLOT(simulateCreateBankAccountAction()));
    connect(m_SimulateGetAddFundsKeyButton, SIGNAL(clicked()), static_cast<BankServerClientTest*>(m_DebuggableStartableStoppableBackend), SLOT(simulateGetAddFundsKeyAction()));
}
BankClientDebugWidget::~BankClientDebugWidget()
{    
}
