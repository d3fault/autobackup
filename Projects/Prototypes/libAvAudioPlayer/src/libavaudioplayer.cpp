#include "libavaudioplayer.h"

libAvAudioPlayer::libAvAudioPlayer(QObject *parent) :
    QObject(parent)
{
}
void libAvAudioPlayer::initAndPlay()
{
    if(!init())
        return;
    QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
}
bool libAvAudioPlayer::init()
{
    m_CurrentFrameCount = 0;

    //activate it
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    //open input. file or url
    if(av_open_input_file(&m_InputFormatCtx, "/home/d3fault/test/vision.avi", NULL, 0, NULL) != 0)
    {
        emit d("unable to open input file");
        return false;
    }
    emit d("opened input");
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

    return true;
}
void libAvAudioPlayer::getFrame()
{
    if(av_read_frame(m_InputFormatCtx, m_InputFramePacket) < 0)
    {
        //TODO: done reading. for now, just return
        return;
    }

    //check to see if it's our target video stream
    if(m_InputFramePacket->stream_index == m_AudioStreamIndex)
    {
        ++m_CurrentFrameCount;
        emit d("read audio frame #" + QString::number(m_CurrentFrameCount));

        int packetSize = m_InputFramePacket->size;
        int numBytesUsed = 0;

        while(packetSize > 0)
        {
            //now decode the frame
            //decodedSize = avcodec_decode_video2(m_InputCodecCtx, m_NativeFormatDecodedFrame, &frameFinished, m_InputFramePacket);
            m_InputFramePacket->data += numBytesUsed;
            int outputBufferSize = m_DecodedAudioBuffer->size();
            if(packetSize < FF_INPUT_BUFFER_PADDING_SIZE)
            {
                return;
            }
            numBytesUsed = avcodec_decode_audio3(m_InputCodecCtx, (int16_t*)m_DecodedAudioBuffer->data(), &outputBufferSize, m_InputFramePacket);
            if(numBytesUsed < 0)
            {
                //error
                emit d("error in num bytes used");
                return;
            }
            if(outputBufferSize > 0)
            {
                //a frame has been decoded
                //TODO: queue it to be played, memcpy or something. data(), outputBufferSize.
                break;
            }
            packetSize -= numBytesUsed;
        }
    }
    av_free_packet(m_InputFramePacket);

    QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
}
