#ifndef LIBAVDECODER_H
#define LIBAVDECODER_H

#include <QObject>
#include <QtMultimedia/QVideoFrame> //TODO: might need a new/custom video frame object that also holds the pts/dts data for synchronizer to work with

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#define IMAGE_PIX_FMT PIX_FMT_RGB32

class LibAvDecoder : public QObject
{
    Q_OBJECT
public:
    explicit LibAvDecoder(QObject *parent = 0);
private:
    bool setupLibAv();

    //libav members
    AVFormatContext *m_InputFormatCtx;
    AVCodecContext  *m_InputVideoCodecCtx;
    AVCodec         *m_InputVideoCodec;
    AVCodecContext  *m_InputAudioCodecCtx;
    AVCodec         *m_InputAudioCodec;
    AVFrame         *m_NativeFormatDecodedVideoFrame;
    AVFrame         *m_SwScaledDecodedVideoFrame;
    AVPacket        *m_InputFramePacket;
    SwsContext      *m_SwScaleCtx;

    //libav locals
    int             m_VideoStreamIndex;
    int             m_AudioStreamIndex;
    double          m_FrameRate;
    unsigned int    m_NativeFormatDecodedFrameSize;
    qint16          m_DecodedAudioBuffer[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int    m_SwScaledDecodedFrameSize;
    quint8          *m_NativeFormatDecodedFrameBuffer;
    quint8          *m_SwScaledDecodedFrameBuffer;
signals:
    void audioFrameDecoded(const QByteArray &);
    void videoFrameDecoded(const QVideoFrame &);
    void d(const QString &);
public slots:
    void init();
    void play();
    void destroy();
private slots:
    void getFrame();
};

#endif // LIBAVDECODER_H
