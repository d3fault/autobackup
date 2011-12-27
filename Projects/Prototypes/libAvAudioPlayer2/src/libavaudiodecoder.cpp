#include "libavaudiodecoder.h"

libAvAudioDecoder::libAvAudioDecoder(ThreadSafeQueueByMutex *decodedAudioBuffer) :
    m_Initialized(false), m_InitFailedSoDontTryAgain(false)
{
    m_SharedDecodedAudioBuffer = decodedAudioBuffer;
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

        QMetaObject::invokeMethod(this, "demuxFrame", Qt::QueuedConnection);
    }
    else
    {
        emit d("failed to init once so not trying again");
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
    int amountCopied = qMin(bufSize, m_MuxedStream.size());
    memcpy((void*)buf, (const void*)m_MuxedStream.data(), amountCopied);
    m_MuxedStream.remove(0, amountCopied);
    return amountCopied;
}
void libAvAudioDecoder::handleNewDataAvailable(QByteArray newData)
{
    //queue it to be decoded later? maybe "de-Stream" it via libav and then queue it to be decoded later? (so we are ONLY processing audio stream) -------- NO, see below
    //in any case, we don't need to mutex our buffer since this slot is called from this thread, yay finally a payoff for doing getFrame() as a slot lawl (pause/stop still apply but i never code those in prototypes fuck the police)
    //we will not be de-stream'ing it here, libav calls readPackets expecting a file and then we can de-stream it and put it in yet another queue to later be decoded
    m_MuxedStream.append(newData);
    if(!m_Initialized && m_MuxedStream.size() >= AMOUNT_TO_BUFFER_BEFORE_STARTING)
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
    m_AllocPutByteOwnedBuffer = (quint8*)av_malloc(INPUT_STREAM_BUFFER_SIZE);
    m_InputFormatCtx->pb = av_alloc_put_byte(m_AllocPutByteOwnedBuffer,INPUT_STREAM_BUFFER_SIZE,0,this, &libAvAudioDecoder::staticReadPackets,NULL,NULL);
    if(!m_InputFormatCtx->pb)
    {
        emit d("error allocating pb");
        return false;
    }

    AVFormatParameters ap;
    memset(&ap, 0, sizeof(ap));
    ap.prealloced_context = 1;

    m_InputFormatCtx->iformat = av_find_input_format("avi"); //meh, i guess i can do URL string detection and just swap it for whatever. .avi, .mkv, .mp3 (only in this "test".. but i wish i didn't have to say :(

#if 0
    AVProbeData pd;
    pd.filename = "";
    pd.buf = (unsigned char*)m_MuxedStream.data();
    pd.buf_size = AMOUNT_TO_BUFFER_BEFORE_STARTING;

    int score=0;
    m_InputFormatCtx->iformat = av_probe_input_format2(&pd, 1, &score);

    if(!m_InputFormatCtx->iformat)
    {
        emit d("iformat is null");
        return false;
    }
    emit d("probed input format: " + QString(m_InputFormatCtx->iformat->name));

    emit d("probe score: " + QString::number(m_InputFormatCtx->iformat->read_probe(&pd)));
#endif

    int r = av_open_input_stream(&m_InputFormatCtx, m_InputFormatCtx->pb, "stream", m_InputFormatCtx->iformat, &ap);
    if(r != 0)
    {
        emit d("error opening input stream: " + QString::number(r));
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

    m_TempDecodedAudioBuffer = new QByteArray(((AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2), '0');

    //create AVPacket instance
    m_MuxedInputFramePacket = new AVPacket();

    m_DeMuxedAudioFramePacket = new AVPacket();
    av_init_packet(m_DeMuxedAudioFramePacket); //we only need to do this once, since we're manually maintaining the data pointer and size inside it.

    emit onSpecGathered(m_InputCodecCtx->sample_rate, m_InputCodecCtx->channels, 16);

    return true;
}
void libAvAudioDecoder::demuxFrame()
{
    //TODO: api-example.c shows me how to do this using a byte buffer as input. will stream_index get updated appropriately though???? i think i need to do av_read_frame, then put the video stream in a queue and the audio stream in a queue. then SOMEWHERE ELSE is where i mimic api-example.c by using a byte buffer as input. right here i think i just av_read_frame and queue the results appropriately... i guess..?

    if(av_read_frame(m_InputFormatCtx, m_MuxedInputFramePacket) < 0)
    {
        //TODO: done reading. for now, just return
        //TODO: maybe NOT done, we might just need to wait for curlDownloader to send us more data???
        return;
    }
    //check to see if it's our target video stream
    if(m_MuxedInputFramePacket->stream_index == m_AudioStreamIndex)
    {
        m_DeMuxedAudioStream.append((const char*)m_MuxedInputFramePacket->data, m_MuxedInputFramePacket->size);
        QMetaObject::invokeMethod(this, "decodeAllQueuedDemuxedAudio", Qt::QueuedConnection);
    }
    //TODO: else if == videoStream, do the same shit. or maybe just decode the video frame here and queue it for later, since i don't have problems with video like i do with audio. 1 frame in = 1 frame out

    av_free_packet(m_MuxedInputFramePacket);

    QMetaObject::invokeMethod(this, "demuxFrame", Qt::QueuedConnection);
}
void libAvAudioDecoder::decodeAllQueuedDemuxedAudio()
{
    //not ALL queued demux'd audio, but as much as we possibly can. once it gets below a certain threshold, we return until demuxFrame gives us more data to work with/calls us again... then we use that leftover data in the beginning of the next call to us

    int packetSize;
    do
    {
        packetSize = m_DeMuxedAudioFramePacket->size = m_DeMuxedAudioStream.size();
        if (packetSize < AUDIO_REFILL_THRESH)
        {
            return; //wait for more data to be queued, which will call us
        }
        m_DeMuxedAudioFramePacket->data = (uint8_t*)m_DeMuxedAudioStream.data();

        int outputBufferSize = m_TempDecodedAudioBuffer->size(); //we re-declare it locally because decdode() modifies it as output

        int numBytesUsed = avcodec_decode_audio3(m_InputCodecCtx, (int16_t*)m_TempDecodedAudioBuffer->data(), &outputBufferSize, m_DeMuxedAudioFramePacket);
        if(numBytesUsed < 0)
        {
            //error
            emit d("error in num bytes used");
            return;
        }
        if(outputBufferSize > 0)
        {
            //m_DecodedAudioBuffer.append(*m_TempDecodedAudioBuffer, outputBufferSize);
            //QByteArray *decodedAudioForSynchronizer = new QByteArray(*m_TempDecodedAudioBuffer, outputBufferSize);
            //emit onAudioDataDecoded(*decodedAudioForSynchronizer);
            //TODO: make sure synchronizer deallocates this when appropriate. but maybe NOT, since it uses implicit sharing my append() in synchronizer should point to this same data? idfk. maybe a shared/mutex'd queue might be better so we don't alloc all the damn time...
            m_SharedDecodedAudioBuffer->append(*m_TempDecodedAudioBuffer, outputBufferSize);
            //should we emit something here? probably not, fuck it. if anything, just the outputBufferSize
        }
        m_DeMuxedAudioStream.remove(0, numBytesUsed);

    }while(packetSize > 0);
}
