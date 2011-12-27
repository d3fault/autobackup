#ifndef LIBAVAUDIODECODER_H
#define LIBAVAUDIODECODER_H

#include <QObject>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#define IMAGE_PIX_FMT PIX_FMT_RGB32
#define AUDIO_REFILL_THRESH 4096
#define INPUT_STREAM_BUFFER_SIZE 20480 //idfk, 4kb is probably good enough for audio, but what about video? will libav call my readPackets multiple times if my buffer size here is too small? idfk.
#define AMOUNT_TO_BUFFER_BEFORE_STARTING INPUT_STREAM_BUFFER_SIZE //4kb of data should be enough to read the header

class libAvAudioDecoder : public QObject
{
    Q_OBJECT
public:
    explicit libAvAudioDecoder(QObject *parent = 0);
private:
    quint8 *m_MuxedInputStream;
    QByteArray m_MuxedStream;
    QByteArray m_DeMuxedAudioStream;
    //QByteArray m_DecodedAudioBuffer; //all of our decoded audio. m_TempDecodedAudioBuffer copies itself into here after each decode(). actually we just emit it to Synchronizer.
    bool m_Initialized;
    bool m_InitFailedSoDontTryAgain;
    bool actualInit();
    static int staticReadPackets(void *opaque, uint8_t *buf, int bufSize);
    int readPackets(uint8_t *buf, int buf_size);

    //libav members
    AVFormatContext *m_InputFormatCtx;
    AVCodecContext  *m_InputCodecCtx;
    AVCodec         *m_InputCodec;
    QByteArray      *m_TempDecodedAudioBuffer; //libav writes directly into this buffer
    AVPacket        *m_MuxedInputFramePacket;
    AVPacket        *m_DeMuxedAudioFramePacket;
    SwsContext      *m_SwScaleCtx;

    //libav locals
    int             m_CurrentFrameCount;
    int             m_AudioStreamIndex;
signals:
    void onSpecGathered(int sampleRate, int numChannels, int sampleSize);
    void onAudioDataDecoded(QByteArray audioData);
    void d(const QString &);
public slots:
    void initAndPlay();
    void handleNewDataAvailable(QByteArray newData);
private slots:
    void demuxFrame();
    void decodeAllQueuedDemuxedAudio();
};

#endif // LIBAVAUDIODECODER_H
