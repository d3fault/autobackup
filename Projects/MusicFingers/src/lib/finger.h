#ifndef FINGER_H
#define FINGER_H

#include <QObject>

class QPropertyAnimation;

class MusicFingersSynthesizer;

class Finger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int position READ position WRITE setPosition)
public:
    enum FingerEnum
    {
        LeftPinky_A0 = 0
      , A0_LeftPinky = LeftPinky_A0

      , LeftRing_A1 = 1
      ,  A1_LeftRing = LeftRing_A1

      , LeftMiddle_A2
      , A2_LeftMiddle = LeftMiddle_A2

      , LeftIndex_A3
      , A3_LeftIndex

      , LeftThumb_A4
      , A4_LeftThumb = LeftThumb_A4

      , RightThumb_A5
      , A5_RightThumb = RightThumb_A5

      , RightIndex_A6
      , A6_RightIndex = RightIndex_A6

      , RightMiddle_A7
      , A7_RightMiddle = RightMiddle_A7

      , RightRing_A8
      , A8_RightRing = RightRing_A8

      , RightPinky_A9
      , A9_RightPinky = RightPinky_A9
    };

    Finger(FingerEnum finger, int initialPosition, QObject *parent = Q_NULLPTR);

    void animatedMoveFingerPosition(int newPosition);

    int position() const;
    void setPosition(int newPosition);
private:
    FingerEnum m_Finger;
    int m_Position;
    QPropertyAnimation *m_FingerMovementAnimation;
};

#endif // FINGER_H
