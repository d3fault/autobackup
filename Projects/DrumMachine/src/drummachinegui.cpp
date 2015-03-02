#include "drummachinegui.h"

#include <QKeyEvent>

DrumMachineGui::DrumMachineGui(QWidget *parent)
    : QWidget(parent)
{ }
void DrumMachineGui::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z)
    {
        emit alphabeticalKeyPressDetected(keyEvent->key());
        return;
    }
    QWidget::keyPressEvent(keyEvent);
}
