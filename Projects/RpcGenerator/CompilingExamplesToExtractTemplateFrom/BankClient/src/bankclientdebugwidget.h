#ifndef BANKCLIENTDEBUGWIDGET_H
#define BANKCLIENTDEBUGWIDGET_H

#include <QtGui/QWidget>
#include <QPushButton>

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
    QPushButton *m_SimulateCreateBankAccountButton;
    QPushButton *m_SimulateGetAddFundsKeyButton;
};

#endif // BANKCLIENTDEBUGWIDGET_H
