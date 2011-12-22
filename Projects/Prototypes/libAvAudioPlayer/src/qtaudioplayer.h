#ifndef QTAUDIOPLAYER_H
#define QTAUDIOPLAYER_H

#include <QObject>
#include <QAudioOutput>
#include <QIODevice>
#include <QTimer>

class ThreadSafeQueueByMutex;

class QtAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QtAudioPlayer(ThreadSafeQueueByMutex *queue);
private:
    ThreadSafeQueueByMutex *m_Queue;
    void actualStart();
    bool m_Started;
    bool m_SpecSet;

    QAudioOutput *m_AudioOutput;
    QAudioFormat m_Format;
    QIODevice* m_AudioBuffer;
    QTimer *m_Timer;
public slots:
    void setAudioSpec(int sampleRate, int numChannels, int sampleSize);
    void start();
private slots:
    void fillAudioBuffer();
};

#endif // QTAUDIOPLAYER_H
