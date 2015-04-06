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
bool Finger::isValidFingerId(int fingerId)
{
    return (fingerId > -1 && fingerId < 10);
}
Finger::FingerEnum Finger::fingerIdToFingerEnum(int fingerId)
{
    switch(fingerId)
    {
    case 0:
        return Finger::A0_LeftPinky;
    case 1:
        return Finger::A1_LeftRing;
    case 2:
        return Finger::A2_LeftMiddle;
    case 3:
        return Finger::A3_LeftIndex;
    case 4:
        return Finger::A4_LeftThumb;
    case 5:
        return Finger::A5_RightThumb;
    case 6:
        return Finger::A6_RightIndex;
    case 7:
        return Finger::A7_RightMiddle;
    case 8:
        return Finger::A8_RightRing;
    case 9:
        return Finger::A9_RightPinky;
    default:
        return Finger::A0_LeftPinky; //should never happen, caller should (and does atm) sanitize before calling
    }
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


