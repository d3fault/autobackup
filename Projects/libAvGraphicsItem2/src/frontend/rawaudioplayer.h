#ifndef RAWAUDIOPLAYER_H
#define RAWAUDIOPLAYER_H

#include <QObject>

class RawAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit RawAudioPlayer(QObject *parent = 0);
    void start();
signals:
public slots:
    void playAudio(const QByteArray &);
};

#endif // RAWAUDIOPLAYER_H
