#include "bankclientdebugwidget.h"

BankClientDebugWidget::BankClientDebugWidget(BankServerClientTest *bankClientTest, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(bankClientTest, parent)
{ }
void BankClientDebugWidget::addButtonsToLayoutAndConnectToBackend()
{
    m_UsernameLineEdit = new QLineEdit("asdfJKL692");
    m_SimulateCreateBankAccountButton = new QPushButton("Simulate Create Bank Account Action");
    m_SimulateGetAddFundsKeyButton = new QPushButton("Simulate Get Add Funds Key Action");

    m_Layout->addWidget(m_UsernameLineEdit);
    m_Layout->addWidget(m_SimulateCreateBankAccountButton);
    m_Layout->addWidget(m_SimulateGetAddFundsKeyButton);

    connect(m_SimulateCreateBankAccountButton, SIGNAL(clicked()), this, SLOT(handleSimulateCreateBankAccountClicked()));
    connect(m_SimulateGetAddFundsKeyButton, SIGNAL(clicked()), this, SLOT(handleSimulateGetAddFundsKeyClicked()));
}
void BankClientDebugWidget::handleSimulateCreateBankAccountClicked()
{
    if(m_UsernameLineEdit->text().trimmed().length() > 0)
    {
        QMetaObject::invokeMethod(static_cast<BankServerClientTest*>(m_DebuggableStartableStoppableBackend), "simulateCreateBankAccountAction", Qt::QueuedConnection, Q_ARG(QString, m_UsernameLineEdit->text().trimmed()));
    }
    else
    {
        this->handleD("enter username");
    }
}
void BankClientDebugWidget::handleSimulateGetAddFundsKeyClicked()
{
    if(m_UsernameLineEdit->text().trimmed().length() > 0)
    {
        QMetaObject::invokeMethod(static_cast<BankServerClientTest*>(m_DebuggableStartableStoppableBackend), "simulateGetAddFundsKeyAction", Qt::QueuedConnection, Q_ARG(QString, m_UsernameLineEdit->text().trimmed()));
    }
    else
    {
        this->handleD("enter username");
    }
}
BankClientDebugWidget::~BankClientDebugWidget()
{
}
