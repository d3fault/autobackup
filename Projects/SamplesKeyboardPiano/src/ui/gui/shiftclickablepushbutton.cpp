#include "shiftclickablepushbutton.h"

#include <QEvent>
#include <QApplication>
#include <QMouseEvent>
#include <QKeySequence>

ShiftClickablePushButton::ShiftClickablePushButton(int keyboardKey, QWidget *parent)
    : QPushButton(QKeySequence(keyboardKey).toString(), parent)
    , m_KeyboardKey(keyboardKey)
{ }
bool ShiftClickablePushButton::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent)
        {
            if(mouseEvent->button() == Qt::LeftButton)
            {
                bool shiftIsPressed = (QApplication::keyboardModifiers() & Qt::ShiftModifier);
                emit alphabeticalKeyPressDetected(m_KeyboardKey, shiftIsPressed);
            }
        }
    }
    return QWidget::eventFilter(object, event);
}
