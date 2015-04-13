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
    return ((fingerId >= 0 && fingerId <= 5) || (fingerId >= 8 && fingerId <= 11)); //hardcoded. arduino micro specific. 0-5 and 8-11

    //TODOreq: non-hardcoed config
    //return (fingerId > -1 && fingerId < 10);
}
Finger::FingerEnum Finger::fingerIdToFingerEnum(int fingerId)
{
    //hardcoded. arduino micro specific. 0-5 and 8-11 ... translated into "0-9" in no specific order (yet)
    switch(fingerId)
    {
    case 0:
        return Finger::RightPinky_A9;
    case 1:
        return Finger::RightThumb_A5;
    case 2:
        return Finger::RightRing_A8;
    case 3:
        return Finger::RightIndex_A6;
    case 4:
        return Finger::RightMiddle_A7;
    case 5:
        return Finger::LeftPinky_A0;
    case 8:
        return Finger::LeftRing_A1;
    case 9:
        return Finger::LeftThumb_A4;
    case 10:
        return Finger::LeftIndex_A3;
    case 11:
        return Finger::LeftIndex_A3;
    default:
        return Finger::A0_LeftPinky; //should never happen, caller should (and does atm) sanitize before calling
    }

#if 0 //TODOreq: better (non-hardcoded) configuration. Where do I want the pin-map-config shit to go? On my Arduino Micro, it was more convenient to use A 0-5 and 11, 10, 9, 8 rather than an easier to understand 0-9. The ordering is also not 0->9 left->right like I wanted, but whatever. I could (and probably will) rewrite this method and just hardcode the config for now, but ideally it'd be a runtime and/or profile thing
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
#endif
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


