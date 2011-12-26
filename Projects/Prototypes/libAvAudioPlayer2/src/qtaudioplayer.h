#ifndef QTAUDIOPLAYER_H
#define QTAUDIOPLAYER_H

#include <QObject>

class QtAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit QtAudioPlayer(QObject *parent = 0);
signals:
    void needAudio(int bytesNeeded);
public slots:
    void init();
    void setAudioSpec(int sampleRate, int numChannels, int sampleSize);
    void handleNewAudioBytes(QByteArray newAudioBytes);
};

#endif // QTAUDIOPLAYER_H
