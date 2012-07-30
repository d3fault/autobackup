#ifndef BANKCLIENTDEBUGWIDGET_H
#define BANKCLIENTDEBUGWIDGET_H

#include <QtGui/QWidget>
#include <QPushButton>
#include <QLineEdit>

#include "idebuggablestartablestoppablefrontend.h"
#include "bankserverclienttest.h"

class BankClientDebugWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT
public:
    BankClientDebugWidget(BankServerClientTest *bankClientTest, QWidget *parent = 0);
    void addButtonsToLayoutAndConnectToBackend();
    ~BankClientDebugWidget();
private:
    QLineEdit *m_UsernameLineEdit;
    QPushButton *m_SimulateCreateBankAccountButton;
    QPushButton *m_SimulateGetAddFundsKeyButton;
private slots:
    void handleSimulateCreateBankAccountClicked();
    void handleSimulateGetAddFundsKeyClicked();
};

#endif // BANKCLIENTDEBUGWIDGET_H
