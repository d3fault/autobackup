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
    bool m_Initialized;
    void initialize();
    QThread *m_DecoderThread;
    LibAvDecoder *m_Decoder;
    QThread *m_SynchronizerThread;
    AudioVideoSynchronizer *m_Synchronizer;
signals:
    void frameReadyToBePresented(const QVideoFrame &);
    void audioReadyToBePresented(const QByteArray &);
public slots:
    void start();
};

#endif // LIBAVPLAYERBACKEND_H
