#include "bankserverclientdebugwidget.h"

BankServerClientDebugWidget::BankServerClientDebugWidget(QWidget *parent)
    : QWidget(parent)
{ }
void BankServerClientDebugWidget::addButtonsToLayoutAndConnectToBackend()
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
void BankServerClientDebugWidget::handleSimulateCreateBankAccountClicked()
{
    if(m_UsernameLineEdit->text().trimmed().length() > 0)
    {
        emit simulateCreateBankAccountActionRequested(m_UsernameLineEdit->text());
        //QMetaObject::invokeMethod(static_cast<RpcBankServerClientTest*>(m_DebuggableStartableStoppableBackend), "simulateCreateBankAccountAction", Qt::QueuedConnection, Q_ARG(QString, m_UsernameLineEdit->text().trimmed()));
    }
    else
    {
        this->handleD("enter username");
    }
}
void BankServerClientDebugWidget::handleSimulateGetAddFundsKeyClicked()
{
    if(m_UsernameLineEdit->text().trimmed().length() > 0)
    {
        emit simulateGetAddFundsKeyActionRequested(m_UsernameLineEdit->text());
        //QMetaObject::invokeMethod(static_cast<RpcBankServerClientTest*>(m_DebuggableStartableStoppableBackend), "simulateGetAddFundsKeyAction", Qt::QueuedConnection, Q_ARG(QString, m_UsernameLineEdit->text().trimmed()));
    }
    else
    {
        this->handleD("enter username");
    }
}
void BankServerClientDebugWidget::handleBusinessInitialized()
{
    handleD("gui noticed business has initialized");
}
void BankServerClientDebugWidget::handleBusinessStarted()
{
    m_BusinessStarted = true;
    enableUserInteraction();
    handleD("gui noticed business has started");
}
void BankServerClientDebugWidget::handleBusinessStopped()
{
    m_BusinessStarted = false;
    enableUserInteraction();
    handleD("gui noticed business has stopped");
}
