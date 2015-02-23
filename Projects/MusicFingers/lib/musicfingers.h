#ifndef MUSICFINGERS_H
#define MUSICFINGERS_H

#include <QObject>

#define MUSIC_FINGERS_RANGE 1024

class MusicFingers : public QObject
{
    Q_OBJECT
public:
    enum Finger
    {
          LeftPinky0
        , LeftRing1
        , LeftMiddl2
        , LeftIndex3
        , LeftThumb4
        , RightThumb5
        , RightIndex6
        , RightRing7
        , RightPinky8
    };
    explicit MusicFingers(QObject *parent = 0);
signals:
    void presentAudioFrame(const QByteArray &audioFrame);
public slots:
    void moveFinger(Finger fingerToMove);
};

#endif // MUSICFINGERS_H
