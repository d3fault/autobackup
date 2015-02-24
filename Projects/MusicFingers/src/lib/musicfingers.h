#ifndef MUSICFINGERS_H
#define MUSICFINGERS_H

#include <QObject>
#include <QHash>
#include <QtMultimedia/QAudio>

#define MUSIC_FINGERS_MIN_POSITION 0
#define MUSIC_FINGERS_MAX_POSITION 1023

class QAudioOutput;
class QIODevice;

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
private:
    QHash<Finger::FingerEnum, int /*position*/> m_Fingers;
    QAudioOutput *m_AudioOutput;
    QIODevice *m_AudioOutputIoDevice;
    int m_TimeInBytesForMySineWave;

    QByteArray synthesizeAudioUsingFingerPositions(int numBytesToSynthesize_aka_audioOutputPeriodSize);
signals:
    void presentAudioFrame(const QByteArray &audioFrame);
public slots:
    void moveFinger(Finger::FingerEnum fingerToMove, int newPosition);
private slots:
    void handleAudioOutputStateChanged(QAudio::State newState);
    void handleAudioOutputNotifiedMeItWantsData();
};

#endif // MUSICFINGERS_H
