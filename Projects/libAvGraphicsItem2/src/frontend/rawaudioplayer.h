#ifndef RAWAUDIOPLAYER_H
#define RAWAUDIOPLAYER_H

#include <QObject>
#include <QtMultimedia/QAudioOutput>

class RawAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit RawAudioPlayer(QObject *parent = 0);
    void init();
private:
    QAudioOutput *m_AudioOutput;
signals:
public slots:
    void playAudio(const QByteArray &);
};

#endif // RAWAUDIOPLAYER_H
