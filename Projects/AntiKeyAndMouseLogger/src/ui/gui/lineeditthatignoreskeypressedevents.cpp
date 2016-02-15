#include "lineeditthatignoreskeypressedevents.h"

#include <QKeyEvent>

#include "antikeyandmouselogger.h"

LineEditThatIgnoresKeyPressedEvents::LineEditThatIgnoresKeyPressedEvents(QWidget *parent)
    : QLineEdit(parent)
{ }
void LineEditThatIgnoresKeyPressedEvents::keyPressEvent(QKeyEvent *keyEvent)
{
    QString key = AntiKeyAndMouseLogger::QtKeyToString(keyEvent->key());
    if(!AntiKeyAndMouseLogger::isTypeableOnUsKeyboardWithoutNeedingShiftKey(key))
    {
        QLineEdit::keyPressEvent(keyEvent);
        return;
    }
    keyEvent->setAccepted(false); //we ignore the key press event so that the parent widget gets it instead
}
