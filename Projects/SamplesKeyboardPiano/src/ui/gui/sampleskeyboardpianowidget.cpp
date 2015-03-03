#include "sampleskeyboardpianowidget.h"

#include <QKeyEvent>

SamplesKeyboardPianoWidget::SamplesKeyboardPianoWidget(QWidget *parent)
    : QWidget(parent)
{ }
void SamplesKeyboardPianoWidget::keyPressEvent(QKeyEvent *keyEvent, bool shiftKeyPressed)
{
    if(keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z)
    {
        bool shiftKeyPressed = (keyEvent->modifiers() & Qt::ShiftModifier);
        emit alphabeticalKeyPressDetected(keyEvent->key(), shiftKeyPressed);
        return;
    }
    QWidget::keyPressEvent(keyEvent);
}
