#ifndef MUSICFINGERS_H
#define MUSICFINGERS_H

#include <QObject>

#define MUSIC_FINGERS_MIN_POSITION 0
#define MUSIC_FINGERS_MAX_POSITION 1023

class Finger
{
public:
    enum FingerEnum
    {
        LeftPinky0
      , LeftRing1
      , LeftMiddl2
      , LeftIndex3
      , LeftThumb4
      , RightThumb5
      , RightIndex6
      , RightMiddle
      , RightRing7
      , RightPinky8
    };
};

class MusicFingers : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingers(QObject *parent = 0);
signals:
    void presentAudioFrame(const QByteArray &audioFrame);
public slots:
    void moveFinger(Finger::FingerEnum fingerToMove, int newPosition);
};

#endif // MUSICFINGERS_H
