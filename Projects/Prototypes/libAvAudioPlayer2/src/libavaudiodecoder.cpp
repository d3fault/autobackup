#include "libavaudiodecoder.h"

libAvAudioDecoder::libAvAudioDecoder(QObject *parent) :
    QObject(parent)
{
}
void libAvAudioDecoder::init()
{
    //TODOreq: we should not call init until the loader has given us some data to work with. actualInit() needs data to work with.
    if(!actualInit())
        return;
}
int libAvAudioDecoder::staticReadPackets(void *opaque, uint8_t *buf, int bufSize)
{
    return static_cast<libAvAudioDecoder*>(opaque)->readPackets(buf, bufSize);
}
int libAvAudioDecoder::readPackets(uint8_t *buf, int bufSize)
{
    //buf_size is the size of the buffer we are going to fill
    //returns the number of bytes actually copied (how much is available based on our loader)
}
void libAvAudioDecoder::handleNewDataAvailable(QByteArray newData)
{
    //queue it to be decoded later? maybe "de-Stream" it via libav and then queue it to be decoded later? (so we are ONLY processing audio stream) -------- NO, see below
    //in any case, we don't need to mutex our buffer since this slot is called from this thread, yay finally a payoff for doing getFrame() as a slot lawl (pause/stop still apply but i never code those in prototypes fuck the police)
    //we will not be de-stream'ing it here, libav calls readPackets expecting a file and then we can de-stream it and put it in yet another queue to later be decoded
}
bool libAvAudioDecoder::actualInit()
{
    m_CurrentFrameCount = 0;

    //activate it
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    //open input. file or url

    m_InputFormatCtx = avformat_alloc_context();
    m_InputFormatCtx->flags |= AVFMT_NOFILE|AVFMT_FLAG_IGNIDX;
    m_InputStreamBuffer = (quint8*)av_malloc(INPUT_STREAM_BUFFER_SIZE);
    m_InputFormatCtx->pb = av_alloc_put_byte(m_InputStreamBuffer,INPUT_STREAM_BUFFER_SIZE,0,this, &libAvAudioDecoder::staticReadPackets,NULL,NULL);
    if(!m_InputFormatCtx->pb)
    {
        emit d("error allocating pb");
        return false;
    }

    AVFormatParameters ap;
    memset(&ap, 0, sizeof(ap));
    ap.prealloced_context = 1;

    int r = av_open_input_stream(&m_InputFormatCtx, m_InputFormatCtx->pb, "stream", m_InputFormatCtx->iformat, &ap);
    if(r != 0)
    {
        emit d("error opening input stream");
        return false;
    }
    emit d("opened input stream");
#if 0
    if(av_open_input_file(&m_InputFormatCtx, "/home/d3fault/test/lights.mp3", NULL, 0, NULL) != 0)
    {
        emit d("unable to open input file");
        return false;
    }
    emit d("opened input");
#endif
    //find stream info
    if(av_find_stream_info(m_InputFormatCtx) < 0)
    {
        emit d("unable to find stream info");
        return false;
    }
    emit d("stream info found");
    //find first audio stream index
    for(unsigned int i = 0; i < m_InputFormatCtx->nb_streams; ++i)
    {
        if(m_InputFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_AudioStreamIndex = i;
            break;
        }
    }
    if(m_AudioStreamIndex == -1)
    {
        emit d("unable to find a audio stream in the input file");
        return false;
    }
    emit d("found audio stream index: " + QString::number(m_AudioStreamIndex));
    //grab the codec-id from the input file's first audio stream
    m_InputCodecCtx = m_InputFormatCtx->streams[m_AudioStreamIndex]->codec;
    //search libav for a decoder matching the input file's first audio stream's codec-id
    m_InputCodec = avcodec_find_decoder(m_InputCodecCtx->codec_id);
    if(m_InputCodec == NULL)
    {
        emit d("unable to find the codec for the input file's first audio stream");
        return false;
    }
    emit d("found a suitable codec: " + QString(m_InputCodec->name));
    if(avcodec_open(m_InputCodecCtx, m_InputCodec) < 0)
    {
        emit d("unable to open the codec for the input file's first audio stream");
        return false;
    }
    emit d("opened codec");

    m_DecodedAudioBuffer = new QByteArray(((AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2), '0');

    //create AVPacket instance
    m_InputFramePacket = new AVPacket();

    emit onSpecGathered(m_InputCodecCtx->sample_rate, m_InputCodecCtx->channels, 16);

    return true;
}
