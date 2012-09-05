#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "idebuggablestartablestoppablefrontend.h"

class mainWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT

public:
    mainWidget(IDebuggableStartableStoppableBackend *debuggableStartableStoppableBackend, QWidget *parent = 0);
protected:
    void addButtonsToLayoutAndConnectToBackend();
};

#endif // MAINWIDGET_H
