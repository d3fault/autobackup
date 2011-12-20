#ifndef LIBAVDECODER_H
#define LIBAVDECODER_H

#include <QObject>
#include <QtMultimedia/QVideoFrame> //TODO: might need a new/custom video frame object that also holds the pts/dts data for synchronizer to work with

class LibAvDecoder : public QObject
{
    Q_OBJECT
public:
    explicit LibAvDecoder(QObject *parent = 0);
signals:
    void audioFrameDecoded(const QByteArray &);
    void videoFrameDecoded(const QVideoFrame &);
public slots:
    void init();
    void play();
};

#endif // LIBAVDECODER_H
