#ifndef BANKSERVERDEBUGWIDGET_H
#define BANKSERVERDEBUGWIDGET_H

#include <QPushButton>

#include "idebuggablestartablestoppablefrontend.h"
#include "bankservertest.h"

class bankServerDebugWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT
public:
    bankServerDebugWidget(BankServerTest *bankServerTest, QWidget *parent = 0);
    ~bankServerDebugWidget();
private:
    QPushButton *m_SimulatePendingBalanceDetectedBroadcast;
    QPushButton *m_SimulateConfirmedBalanceDetectedBroadcast;
protected:
    void addButtonsToLayoutAndConnectToBackend();
};

#endif // BANKSERVERDEBUGWIDGET_H
