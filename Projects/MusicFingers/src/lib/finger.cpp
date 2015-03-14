#include "finger.h"

#include <QPropertyAnimation>
#include <QEasingCurve>

Finger::Finger(FingerEnum finger, int initialPosition, QObject *parent)
    : QObject(parent)
    , m_Finger(finger)
    , m_Position(initialPosition)
    , m_FingerMovementAnimation(new QPropertyAnimation(this, "position", this))
{
    m_FingerMovementAnimation->setEasingCurve(QEasingCurve::OutElastic);
    m_FingerMovementAnimation->setDuration(5); //weird, setting this to 50 makes position jump to a really large negative value (and stay there) intermittently... :-/
}
void Finger::animatedMoveFingerPosition(int newPosition)
{
    m_FingerMovementAnimation->setEndValue(newPosition);
    if(m_FingerMovementAnimation->state() == QAbstractAnimation::Stopped)
    {
        m_FingerMovementAnimation->start();
        return;
    }
    if(m_FingerMovementAnimation->state() == QAbstractAnimation::Paused)
    {
        m_FingerMovementAnimation->resume();
        return;
    }
}
int Finger::position() const
{
    return m_Position;
}
void Finger::setPosition(int newPosition)
{
    m_Position = newPosition;
}

