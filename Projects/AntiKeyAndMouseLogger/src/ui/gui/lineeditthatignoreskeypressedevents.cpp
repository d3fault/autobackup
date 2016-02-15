#include "lineeditthatignoreskeypressedevents.h"

#include <QKeyEvent>

#include "antikeyandmouselogger.h"

LineEditThatIgnoresKeyPressedEvents::LineEditThatIgnoresKeyPressedEvents(QWidget *parent)
    : QLineEdit(parent)
{ }
void LineEditThatIgnoresKeyPressedEvents::keyPressEvent(QKeyEvent *keyEvent)
{
    int key = keyEvent->key();
#if 0
    switch(key)
    {
    //allow SOME nav/control over the line edit, to delete mistakes.
    case Qt::Key_Delete:x
    case Qt::Key_Backspace:
    case Qt::Key_Home:
    case Qt::Key_PageUp:
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_End:
    case Qt::Key_PageDown:
    case Qt::Key_Right:
    case Qt::Key_Down:
        QLineEdit::keyPressEvent(keyEvent);
        return;
    }
#endif
    if(!AntiKeyAndMouseLogger::isTypeableOnUsKeyboard(AntiKeyAndMouseLogger::QtKeyToString(key)))
    {
        //the name of the function is misleading. what we really mean instead of typeable is whether or not we can output them for passwords etc. you could 'type' random ass unicode still. I was gonna do a whitelist instead of a blacklist (above, commente out) but there was too much to think of
        QLineEdit::keyPressEvent(keyEvent);
        return;
    }
    keyEvent->setAccepted(false); //we ignore the key press event so that the parent widget gets it instead
}
