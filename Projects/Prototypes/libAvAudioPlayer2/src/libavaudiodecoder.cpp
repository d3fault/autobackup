#include "libavaudiodecoder.h"

libAvAudioDecoder::libAvAudioDecoder(QObject *parent) :
    QObject(parent), m_Initialized(false), m_InitFailedSoDontTryAgain(false)
{
}
void libAvAudioDecoder::initAndPlay()
{
    if(!m_InitFailedSoDontTryAgain)
    {
        if(!m_Initialized)
        {
            if(!actualInit())
            {
                m_InitFailedSoDontTryAgain = true;
                return;
            }
            m_Initialized = true;
        }

        QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
    }
}
int libAvAudioDecoder::staticReadPackets(void *opaque, uint8_t *buf, int bufSize)
{
    return static_cast<libAvAudioDecoder*>(opaque)->readPackets(buf, bufSize);
}
int libAvAudioDecoder::readPackets(uint8_t *buf, int bufSize)
{
    //buf_size is the size of the buffer we are going to fill
    //returns the number of bytes actually copied (how much is available based on our loader)
    int amountCopied = qMin(bufSize, m_StreamQueue.size());
    memcpy((void*)buf, (const void*)m_StreamQueue.data(), amountCopied);
    m_StreamQueue.remove(0, amountCopied);
    return amountCopied;
}
void libAvAudioDecoder::handleNewDataAvailable(QByteArray newData)
{
    //queue it to be decoded later? maybe "de-Stream" it via libav and then queue it to be decoded later? (so we are ONLY processing audio stream) -------- NO, see below
    //in any case, we don't need to mutex our buffer since this slot is called from this thread, yay finally a payoff for doing getFrame() as a slot lawl (pause/stop still apply but i never code those in prototypes fuck the police)
    //we will not be de-stream'ing it here, libav calls readPackets expecting a file and then we can de-stream it and put it in yet another queue to later be decoded
    m_StreamQueue.append(newData);
    if(!m_Initialized && m_StreamQueue.size() >= AMOUNT_TO_BUFFER_BEFORE_STARTING)
    {
        initAndPlay();
    }
    //TODO: maybe set that eof_reached thing i read about on some mailing list thread back to 0 so av_read_frame will work?
    //TODO: maybe clear/dealloc the newData since it was a pointer/new'd in our curl downloader. as long as it's definitely COPIED into m_StreamQueue, it should be ok to clear/dealloc it now. test this after you get the prototype actually working
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
void libAvAudioDecoder::getFrame()
{
    //TODO: api-example.c shows me how to do this using a byte buffer as input. will stream_index get updated appropriately though???? i think i need to do av_read_frame, then put the video stream in a queue and the audio stream in a queue. then SOMEWHERE ELSE is where i mimic api-example.c by using a byte buffer as input. right here i think i just av_read_frame and queue the results appropriately... i guess..?
#if 0
    if(av_read_frame(m_InputFormatCtx, m_InputFramePacket) < 0)
    {
        //TODO: done reading. for now, just return
        //TODO: maybe NOT done, we might just need to wait for curlDownloader to send us more data???
        return;
    }
#endif

    //check to see if it's our target video stream
    if(m_InputFramePacket->stream_index == m_AudioStreamIndex)
    {
        ++m_CurrentFrameCount;
        emit d("read audio frame #" + QString::number(m_CurrentFrameCount));

        int packetSize = m_InputFramePacket->size;
        int numBytesUsed = 0;

#if 0
        while(packetSize > 0)
        {
            outputBufferSize = ;
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
                m_Queue->append(*m_DecodedAudioBuffer, outputBufferSize);
                //a difference in the example is that it does not break here, but continues looping to see if there is more packet bytes?
                //break;
            }
            packetSize -= numBytesUsed;
            dataPointer += numBytesUsed;
            if(packetSize < 4096)
            {

                break;
                //right here it tells me to get more data... but that consists of me reading another frame... so i want to break
                //but then there's potentially unused packet bytes that i never use... so wtf?
                //i could queue them right after reading them, then deQueue them, and decode them... so RIGHT HERE i'd have something to dequeue from to get more bytes... but in my testing i've never even hit this shit anyways so wtf...
            }
        }
#endif
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
                m_Queue->append(*m_DecodedAudioBuffer, outputBufferSize);
                if(numBytesUsed >= packetSize)
                {
                    break;
                }
                else//else, keep looping because we have more data in the packet?
                {
                    emit d("more data left in this packet");
                }
            }
            packetSize -= numBytesUsed;
        }
    }
    av_free_packet(m_InputFramePacket);

    QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
}
