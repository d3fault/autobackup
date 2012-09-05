#include "mainwidget.h"

mainWidget::mainWidget(IDebuggableStartableStoppableBackend *debuggableStartableStoppableBackend, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(debuggableStartableStoppableBackend, parent)
{ }
void mainWidget::addButtonsToLayoutAndConnectToBackend()
{
}
