#ifndef MUSICFINGERS_H
#define MUSICFINGERS_H

#include <QObject>
#include <QHash>
#include <QtMultimedia/QAudio>

#define MUSIC_FINGERS_MIN_POSITION 0
#define MUSIC_FINGERS_MAX_POSITION 1023
#define FINGERS_INITIAL_POSITION ((MUSIC_FINGERS_MAX_POSITION/2)+1) //Divided by two to put it "in the middle", plus one to put it at "key NOT pressed"

class QAudioOutput;
class QIODevice;

class Finger
{
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
};

class MusicFingers : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingers(QObject *parent = 0);
    static bool isValidFingerId(int fingerId);
    static Finger::FingerEnum fingerIdToFingerEnum(int fingerId);
private:
    QHash<Finger::FingerEnum /*finger*/, int /*position*/> m_Fingers;
    QAudioOutput *m_AudioOutput;
    QIODevice *m_AudioOutputIoDevice;
    double m_TimeInBytesForMySineWave;

    QByteArray synthesizeAudioUsingFingerPositions(int numBytesToSynthesize_aka_audioOutputPeriodSize);
signals:
    void presentAudioFrame(const QByteArray &audioFrame);
    void o(const QString &msg);
    void e(const QString &msg);
    void quitRequested();
public slots:
    void moveFinger(Finger::FingerEnum fingerToMove, int newPosition);
    void queryMusicFingerPosition(int fingerIndex);
private slots:
    void handleAudioOutputStateChanged(QAudio::State newState);
    void writeBytesToAudioOutput();
};

#endif // MUSICFINGERS_H
