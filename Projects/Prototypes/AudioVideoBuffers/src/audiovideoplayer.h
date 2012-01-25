#ifndef AUDIOVIDEOPLAYER_H
#define AUDIOVIDEOPLAYER_H

#include <QObject>
#include <QThread>

#include <curldownloader.h>
#include <localfile.h>
#include <decoder.h>
#include <audioplayer.h>

class GeneratedDataBuffer;

class AudioVideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioVideoPlayer(QObject *parent = 0);
private:
    CurlDownloader *m_Curl;
    LocalFile *m_LocalFile;
    Decoder *m_Decoder;
    AudioPlayer *m_AudioPLayer;

    QThread *m_CurlThread;
    QThread *m_LocalFileThread;
    QThread *m_DecoderThread;
    QThread *m_AudioPLayerThread;
signals:

public slots:
    void initAndProbe();
    void destroy();
};

#endif // AUDIOVIDEOPLAYER_H
