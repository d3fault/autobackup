#include "libavdecoder.h"

LibAvDecoder::LibAvDecoder(QObject *parent) :
    QObject(parent)
{
}
void LibAvDecoder::init()
{
    if(!setupLibAv())
        return;
}
void LibAvDecoder::play()
{
    QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
}
bool LibAvDecoder::setupLibAv()
{
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
    //find first video stream index
    for(unsigned int i = 0; i < m_InputFormatCtx->nb_streams; ++i)
    {
        if(m_InputFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_VideoStreamIndex = i;
            if(m_AudioStreamIndex >= 0)
                break;
        }
        else if(m_InputFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_AudioStreamIndex = i;
            if(m_VideoStreamIndex >= 0)
                break;
        }
    }
    if(m_VideoStreamIndex == -1)
    {
        emit d("unable to find a video stream in the input file");
        return false;
    }
    if(m_AudioStreamIndex == -1)
    {
        emit d("unable to find an audio stream in the input file");
        return false; //TODO: handle video files without audio streams. sync with blank audio.
    }
    emit d("found video stream index: " + QString::number(m_VideoStreamIndex));
    //grab the codec-id from the input file's first video stream
    m_InputVideoCodecCtx = m_InputFormatCtx->streams[m_VideoStreamIndex]->codec;
    //search libav for a decoder matching the input file's first video stream's codec-id
    m_InputVideoCodec = avcodec_find_decoder(m_InputVideoCodecCtx->codec_id);
    if(m_InputVideoCodec == NULL)
    {
        emit d("unable to find the codec for the input file's first video stream");
        return false;
    }
    emit d("found a suitable codec: " + QString(m_InputVideoCodec->name));
    if(avcodec_open(m_InputVideoCodecCtx, m_InputVideoCodec) < 0)
    {
        emit d("unable to open the codec for the input file's first stream");
        return false;
    }
    emit d("opened video codec");

    m_InputAudioCodecCtx = m_InputFormatCtx->streams[m_AudioStreamIndex]->codec;
    m_InputAudioCodec = avcodec_find_decoder(m_InputAudioCodecCtx->codec_id);
    if(m_InputAudioCodecCtx == NULL)
    {
        emit d("unable to find the codec for the input file's first audio stream");
        return false;
    }
    emit d("found a suitable audio codec: " + QString(m_InputAudioCodec->name));
    if(avcodec_open(m_InputAudioCodecCtx, m_InputAudioCodec) < 0)
    {
        emit d("unable to open the codec for the input file's first audio stream");
        return false;
    }
    emit d("opened audio codec");

    m_AudioClockIncrementDenominator = (double)((m_InputAudioCodecCtx->channels * 2) * m_InputAudioCodecCtx->sample_rate);

    //store frame rate
    m_FrameRate = av_q2d(m_InputFormatCtx->streams[m_VideoStreamIndex]->r_frame_rate);
    emit d("video frame rate: " + QString::number(m_FrameRate));
    //hack in example
    if(m_InputVideoCodecCtx->time_base.num>1000 && m_InputVideoCodecCtx->time_base.den==1)
    {
        m_InputVideoCodecCtx->time_base.den=1000;
    }
    //allocate a frame for our decoded image
    m_NativeFormatDecodedVideoFrame = avcodec_alloc_frame();
    if(m_NativeFormatDecodedVideoFrame == NULL)
    {
        emit d("unable to allocate internal frame for decoded image");
        return false;
    }
    emit d("internal frame for decoded image allocated");
    //allocate a frame for our swscale'd decoded image
    m_SwScaledDecodedVideoFrame = avcodec_alloc_frame();
    if(m_SwScaledDecodedVideoFrame == NULL)
    {
        emit d("unable to allocate internal frame for swscale'd decoded image");
        return false;
    }
    emit d("internal frame for swscale'd decoded image allocated");
    //get native format decoded frame size
    m_NativeFormatDecodedFrameSize = avpicture_get_size(m_InputVideoCodecCtx->pix_fmt, m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height);
    if(m_NativeFormatDecodedFrameSize <= 0)
    {
        emit d("zero or negative native format decoded frame size");
        return false;
    }
    emit d("native format decoded frame size: " + QString::number(m_NativeFormatDecodedFrameSize));
    //get swscale'd decoded frame size
    m_SwScaledDecodedFrameSize = avpicture_get_size(IMAGE_PIX_FMT, m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height);
    if(m_SwScaledDecodedFrameSize <= 0)
    {
        emit d("zero or negative swscale'd decoded frame size");
        return false;
    }
    emit d("swscale'd decoded frame size: " + QString::number(m_SwScaledDecodedFrameSize));
    //tell whoever's listening what size/format we're using
    emit videoSizeObtained(m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height, QImage::Format_RGB32); //TODO: dynamically detect format
    //allocate the buffer that will hold the data for the native format decoded frame
    //m_InputFrameBuffer = (uint8_t*)av_malloc(m_DecodedFrameSize);
    m_NativeFormatDecodedFrameBuffer = (quint8*)malloc(m_NativeFormatDecodedFrameSize);
    if(m_NativeFormatDecodedFrameBuffer == NULL)
    {
        emit d("unable to allocate native format decoded frame buffer");
        return false;
    }
    emit d("native format decoded frame buffer allocated");
    //allocate the buffer that will hold the data for the swscale'd decoded frame
    m_SwScaledDecodedFrameBuffer = (quint8*)malloc(m_SwScaledDecodedFrameSize);
    if(m_SwScaledDecodedFrameBuffer == NULL)
    {
        emit d("unable to allocate swscale'd decoded frame buffer");
        return false;
    }
    emit d("swscale'd decoded frame buffer allocated");
    //associate both of the native format frame and our allocated decode frame buffer with each other
    avpicture_fill((AVPicture *)m_NativeFormatDecodedVideoFrame, m_NativeFormatDecodedFrameBuffer, m_InputVideoCodecCtx->pix_fmt, m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height);
    //associate both of the swscale'd frame and our allocated decode frame buffer with each other
    avpicture_fill((AVPicture *)m_SwScaledDecodedVideoFrame, m_SwScaledDecodedFrameBuffer, IMAGE_PIX_FMT, m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height);
    //set up swscale
    //first research if we need to convert the image before giving it to Qt. might not even need it
    m_SwScaleCtx = sws_getContext(m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height, m_InputVideoCodecCtx->pix_fmt, m_InputVideoCodecCtx->width, m_InputVideoCodecCtx->height, IMAGE_PIX_FMT, SWS_BICUBIC, NULL, NULL, NULL);
    //create AVPacket instance
    m_InputFramePacket = new AVPacket();
}
void LibAvDecoder::getFrame()
{
    //get/decode the frame
    //put it on a buffer that the audio/timing thread will use to emit frameReady()

    int frameFinished, decodedSize, currentFrame = 0;
    //read in an av frame. it might be the audio stream or the video stream
    if(av_read_frame(m_InputFormatCtx, m_InputFramePacket) < 0)
    {
        //TODO: done reading. for now, just return
        return;
    }
    //check to see if it's our target video stream
    if(m_InputFramePacket->stream_index == m_VideoStreamIndex)
    {
        emit d("read video frame #" + QString::number(currentFrame));
        //now decode the frame
        decodedSize = avcodec_decode_video2(m_InputVideoCodecCtx, m_NativeFormatDecodedVideoFrame, &frameFinished, m_InputFramePacket);
        if(frameFinished > 0)
        {
            ++currentFrame; //we need to make sure frameFinished before incrementing, becaues previous attempts may have only gotten partial frames
            emit d("decoded frame #" + QString::number(currentFrame) + ", size=" + QString::number(decodedSize));

            //scale to RGB32 for rendering
            sws_scale(m_SwScaleCtx, m_NativeFormatDecodedVideoFrame->data, m_NativeFormatDecodedVideoFrame->linesize, 0, m_InputVideoCodecCtx->height, m_SwScaledDecodedVideoFrame->data, m_SwScaledDecodedVideoFrame->linesize);

            //now render
            emit d("rendering frame #" + QString::number(currentFrame));
            emit frameReady(m_SwScaledDecodedFrameBuffer); //the buffer is what puts it in the QImage and the QVideoFrame.. then renderer dequeue's it as that
        }
    }
    else if(m_InputFramePacket->stream_index == m_AudioStreamIndex)
    {
        if(m_InputFramePacket->pts != AV_NOPTS_VALUE)
        {
            m_AudioClock = av_q2d(m_InputFormatCtx->streams[m_AudioStreamIndex]->time_base) * m_InputFramePacket->pts;
        }

        int actualDecodedBufferSize = sizeof(m_DecodedAudioBuffer); //as input, signifies the maximum size of our decoded audio buffer
        int sizeReadFromInputPacket = avcodec_decode_audio3(m_InputAudioCodecCtx, m_DecodedAudioBuffer, &actualDecodedBufferSize, m_InputFramePacket);
        if(actualDecodedBufferSize > 0) //as output, tells us how big the returned decoded audio buffer is
        {
            m_AudioClock += (((double)actualDecodedBufferSize) / m_AudioClockIncrementDenominator);
        }
        else //actualDecodedBufferSize <= 0
        {
            //no data was returned, get another frame
            //TODO: atm, there is no code below here so just letting it continue onwards has the same effect.
        }
    }
    av_free_packet(m_InputFramePacket);

    //send an event to ourself to schedule the getting of another frame. this is done so we can still use signals to send us a STOP command for example
    QMetaObject::invokeMethod(this, "getFrame", Qt::QueuedConnection);
}
void LibAvDecoder::destroy()
{
    //TODO
}
