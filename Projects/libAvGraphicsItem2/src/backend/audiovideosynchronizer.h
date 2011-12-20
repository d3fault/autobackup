#ifndef AUDIOVIDEOSYNCHRONIZER_H
#define AUDIOVIDEOSYNCHRONIZER_H

#include <QObject>
#include <QtMultimedia/QVideoFrame>

class AudioVideoSynchronizer : public QObject
{
    Q_OBJECT
public:
    explicit AudioVideoSynchronizer(QObject *parent = 0);
signals:
    void audioReadyToBePresented(const QByteArray &);
    void frameReadyToBePresented(const QVideoFrame &);
public slots:
    void init();
    void play();
    void queueAudioFrame(const QByteArray &);
    void queueVideoFrame(const QVideoFrame &);
};

#endif // AUDIOVIDEOSYNCHRONIZER_H
