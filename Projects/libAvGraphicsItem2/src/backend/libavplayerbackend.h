#ifndef LIBAVPLAYERBACKEND_H
#define LIBAVPLAYERBACKEND_H

#include <QObject>
#include <QThread>
#include <QtMultimedia/QVideoFrame>

class LibAvDecoder;
class AudioVideoSynchronizer;

class LibAvPlayerBackend : public QObject
{
    Q_OBJECT
public:
    explicit LibAvPlayerBackend(QObject *parent = 0);
private:
    void privConstructor();
    QThread *m_DecoderThread;
    LibAvDecoder *m_Decoder;
    QThread *m_SynchronizerThread;
    AudioVideoSynchronizer *m_Synchronizer;
signals:
    void onPlay();
    void frameReadyToBePresented(const QVideoFrame &);
    void audioReadyToBePresented(const QByteArray &);
public slots:
    void init();
    void play(); //TODO: stop, pause
};

#endif // LIBAVPLAYERBACKEND_H
