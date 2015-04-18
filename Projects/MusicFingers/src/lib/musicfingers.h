#ifndef MUSICFINGERS_H
#define MUSICFINGERS_H

#include <QObject>
#include <QScopedPointer>
#include <QtMultimedia/QAudio>

#include "finger.h"
#include "musicfingerssynthesizer.h"

#define MUSIC_FINGERS_SAMPLE_RATE 44100

class QAudioOutput;
class QIODevice;

class MusicFingers : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingers(QObject *parent = 0);
private:
    QAudioOutput *m_AudioOutput;
    MusicFingersSynthesizer *m_MusicFingersSynthesizer;
    bool m_AudioOutputHasBeenIdleBefore;
    int m_NumPeriodsToWriteEachTime;
signals:
    void o(const QString &msg);
    void quitRequested();
public slots:
    void moveFinger(Finger::FingerEnum fingerToMove, int newPosition);
    void queryMusicFingerPosition(int fingerIndex);
private slots:
    void handleAudioOutputStateChanged(QAudio::State newState);
};

#endif // MUSICFINGERS_H
