#ifndef LIBAVAUDIOPLAYER_H
#define LIBAVAUDIOPLAYER_H

#include <QObject>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#define IMAGE_PIX_FMT PIX_FMT_RGB32

class ThreadSafeQueueByMutex;

class libAvAudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit libAvAudioPlayer(ThreadSafeQueueByMutex *queue);
private:
    ThreadSafeQueueByMutex *m_Queue;
    bool init();

    //libav members
    AVFormatContext *m_InputFormatCtx;
    AVCodecContext  *m_InputCodecCtx;
    AVCodec         *m_InputCodec;
    QByteArray      *m_DecodedAudioBuffer;
    AVPacket        *m_InputFramePacket;
    SwsContext      *m_SwScaleCtx;

    //libav locals
    int             m_CurrentFrameCount;
    int             m_AudioStreamIndex;
    double          m_FrameRate;
    unsigned int    m_NativeFormatDecodedFrameSize;
    unsigned int    m_SwScaledDecodedFrameSize;
signals:
    void audioSpecGathered(int sampleRate, int numChannels, int sampleSize);
    void d(const QString &);
public slots:
    void initAndPlay();
    void getFrame();
};

#endif // LIBAVAUDIOPLAYER_H
