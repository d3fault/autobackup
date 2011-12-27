//#define PROBE_INPUT 1

#include <QtCore/QCoreApplication>

#include <iostream>

#include <QByteArray>
#include <QFile>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}
using namespace std;


#define INPUT_STREAM_BUFFER_SIZE 16384

QByteArray *fileInMemory;

void myOut(QString output)
{
    std::string out = output.toUtf8().constData();
    std::cout << out << std::endl;
}

int readPacket(void *opaque, uint8_t *buf, int bufSize)
{
    Q_UNUSED(opaque);
    myOut("file in memory bytes remaining: " + QString::number(fileInMemory->size()));
    int size = qMin(bufSize, fileInMemory->size());
    memcpy((void*)buf, (const void*)fileInMemory->data(), size);
    fileInMemory->remove(0, size);
    return size;
}

void loadAviFileIntoMemory()
{
    QFile file("/home/d3fault/test/cont.avi");
    file.open(QIODevice::ReadOnly);
    fileInMemory = new QByteArray(file.readAll());
    file.close();
    myOut("file size in bytes: " + QString::number(fileInMemory->size()));
}
void readAviHeader()
{
    avcodec_init();
    avcodec_register_all();
    av_register_all();

    AVFormatContext *inputFormatCtx = avformat_alloc_context();
    inputFormatCtx->flags |= AVFMT_NOFILE | AVFMT_FLAG_IGNIDX;
    quint8 *putBytedBuffer = (quint8*)av_malloc(INPUT_STREAM_BUFFER_SIZE);
    inputFormatCtx->pb = av_alloc_put_byte(putBytedBuffer,INPUT_STREAM_BUFFER_SIZE,0,NULL,&readPacket,NULL,NULL);
    if(!inputFormatCtx->pb)
    {
        myOut("error av_alloc_put_byte");
        return;
    }

    AVFormatParameters ap;
    memset(&ap, 0, sizeof(ap));
    ap.prealloced_context = 1;

#ifdef PROBE_INPUT
    AVProbeData pd;
    pd.filename = "";
    pd.buf = (unsigned char*)fileInMemory->data();
    pd.buf_size = INPUT_STREAM_BUFFER_SIZE;

    inputFormatCtx->iformat = av_probe_input_format(&pd, 1);

    if(!inputFormatCtx->iformat)
    {
        myOut("iformat is null");
        return;
    }
    myOut("probed input format: " + QString(inputFormatCtx->iformat->name));

    myOut("probe score: " + QString::number(inputFormatCtx->iformat->read_probe(&pd)));
#else
    inputFormatCtx->iformat = av_find_input_format("avi");
#endif

    int r = av_open_input_stream(&inputFormatCtx, inputFormatCtx->pb, "stream", inputFormatCtx->iformat, &ap);
    if(r != 0)
    {
        myOut("error opening input stream: " + QString::number(r));
        return;
    }
    myOut("opened input stream");
    if(av_find_stream_info(inputFormatCtx) < 0)
    {
        myOut("unable to find stream info");
        return;
    }
    myOut("stream info found");
    int audioStreamIndex = -1;
    int videoStreamIndex = -1;
    for(unsigned int i = 0; i < inputFormatCtx->nb_streams; ++i)
    {
        if(inputFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStreamIndex = i;
            if(videoStreamIndex > -1)
                break;
        }
        if(inputFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i;
            if(audioStreamIndex > -1)
                break;
        }
    }
    if(audioStreamIndex == -1)
    {
        myOut("unable to find a audio stream in the input");
        return;
    }
    if(videoStreamIndex == -1)
    {
        myOut("unable to find a video stream in the input");
        return;
    }
    myOut("found audio stream index: " + QString::number(audioStreamIndex));
    myOut("found audio stream index: " + QString::number(videoStreamIndex));
    AVCodecContext *audioCodecCtx = inputFormatCtx->streams[audioStreamIndex]->codec;
    AVCodecContext *videoCodecCtx = inputFormatCtx->streams[videoStreamIndex]->codec;
    AVCodec *audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
    if(audioCodec == NULL)
    {
        myOut("unable to find the codec for the input's first audio stream");
        return;
    }
    if(videoCodec == NULL)
    {
        myOut("unable to find the codec for the input's first video stream");
        return;
    }
    myOut("found a suitable audio codec: " + QString(audioCodec->name));
    myOut("found a suitable video codec: " + QString(videoCodec->name));
    if(avcodec_open(audioCodecCtx, audioCodec) < 0)
    {
        myOut("unable to open the codec for the input's first audio stream");
        return;
    }
    myOut("opened audio codec");
    if(avcodec_open(videoCodecCtx, videoCodec) < 0)
    {
        myOut("unable to open the codec for the input's first video stream");
        return;
    }
    myOut("opened video codec");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    loadAviFileIntoMemory();
    readAviHeader();
    return a.exec();
}
