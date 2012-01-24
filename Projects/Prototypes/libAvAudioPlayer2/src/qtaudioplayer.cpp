#include "qtaudioplayer.h"

QtAudioPlayer::QtAudioPlayer(ThreadSafeQueueByMutex *sharedInputAudioQueue) :
    m_Started(false), m_SpecSet(false)
{
    m_Queue = sharedInputAudioQueue;
}
void QtAudioPlayer::setAudioSpec(int sampleRate, int numChannels, int sampleSize)
{
    m_Format.setFrequency(sampleRate);
    m_Format.setChannelCount(numChannels);
    m_Format.setSampleSize(sampleSize);
    m_Format.setCodec("audio/pcm");
    m_Format.setByteOrder(QAudioFormat::LittleEndian); //TODO: dunno. we should DETECT it really...
    m_Format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_Format))
    {
        m_Format = info.nearestFormat(m_Format);
    }

    m_AudioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), m_Format);
    m_Timer = new QTimer();
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(fillAudioBuffer()));
    m_AudioBuffer = m_AudioOutput->start();

    m_SpecSet = true;
    if(m_Started)
    {
        actualStart();
    }
}
void QtAudioPlayer::play()
{
    m_Started = true;
    if(m_SpecSet)
    {
        actualStart();
    }
}
void QtAudioPlayer::actualStart()
{
    m_Timer->start(8); //15 so we call it about twice per video frame. this way we will always be in sync with the audio with a maximum error of 15ms, not even noticeable to the human eye
}
void QtAudioPlayer::fillAudioBuffer()
{

    if (m_AudioOutput && m_AudioOutput->state() != QAudio::StoppedState)
    {
        int chunks = m_AudioOutput->bytesFree()/m_AudioOutput->periodSize();
        while (chunks)
        {
           QByteArray buffer = m_Queue->deQueue(qMin(m_AudioOutput->bytesFree(), m_AudioOutput->periodSize()));
           int bufferSize = buffer.size();
           if (bufferSize > 0)
           qint64 actuallyWritten = m_AudioBuffer->write(buffer, buffer.size());
           if(actuallyWritten != (qint64)bufferSize || bufferSize != m_AudioOutput->periodSize() || actuallyWritten < 0 /*error gives us -1*/)
           {
               break;
           }
           --chunks;
        }

        qint64 bytesInBuffer = m_AudioOutput->bufferSize() - m_AudioOutput->bytesFree();
        qint64 usInBuffer = (qint64)(1000000) * bytesInBuffer / ( m_Format.channelCount() * m_Format.sampleSize() / 8 ) / m_Format.frequency();
        qint64 usPlayed = m_AudioOutput->processedUSecs() - usInBuffer;

        //TODO: seeking/pausing/stopping needs to add maths here, since processedUSecs is since we called start()

        emit currentPlayingPositionInUsecs(usPlayed); //synchronizer takes this number and determines whether or not to emit a new frame
    }
    else
    {
        m_Timer->stop();
    }
}
