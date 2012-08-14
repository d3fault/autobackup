#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QPushButton>

#include "datastreamdoesnothandleshortreadsbackend.h"
#include "idebuggablestartablestoppablefrontend.h"

class mainWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT

public:
    mainWidget(DataStreamDoesNotHandleShortReadsBackend *debuggableStartableStoppableBackend, QWidget *parent = 0);
protected:
    void addButtonsToLayoutAndConnectToBackend();
private:
    QPushButton *m_SendNextChunkOfMessageButton;
};

#endif // MAINWIDGET_H
