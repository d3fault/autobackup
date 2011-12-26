#ifndef QTAUDIOPLAYER_H
#define QTAUDIOPLAYER_H

#include <QObject>
#include <QtMultimedia/QAudioOutput>
#include <QIODevice>
#include <QTimer>

class QtAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QtAudioPlayer(QObject *parent = 0);
private:
    void actualStart();
    bool m_Started;
    bool m_SpecSet;

    QAudioOutput *m_AudioOutput;
    QAudioFormat m_Format;
    QIODevice* m_AudioBuffer;
    QTimer *m_Timer;
signals:
    void needAudio(int bytesNeeded);
    void d(const QString &);
public slots:
    void init();
    void play();
    void setAudioSpec(int sampleRate, int numChannels, int sampleSize);
    void handleNewAudioBytes(QByteArray newAudioBytes);
private slots:
    void fillAudioBuffer();
};

#endif // QTAUDIOPLAYER_H
