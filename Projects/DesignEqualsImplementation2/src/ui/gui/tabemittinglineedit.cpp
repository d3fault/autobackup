#include "tabemittinglineedit.h"

#include <QEvent>
#include <QKeyEvent>

TabEmittingLineEdit::TabEmittingLineEdit(QWidget *parent)
    : QLineEdit(parent)
{ }
bool TabEmittingLineEdit::event(QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Tab)
        {
            emit tabPressed();
            return true;
        }
    }
    return QLineEdit::event(event);
}
