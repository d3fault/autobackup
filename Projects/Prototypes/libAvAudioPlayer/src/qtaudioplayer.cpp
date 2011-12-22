#include "qtaudioplayer.h"

#include <threadsafequeuebymutex.h>

QtAudioPlayer::QtAudioPlayer(ThreadSafeQueueByMutex *queue) :
    m_Started(false), m_SpecSet(false)
{
    m_Queue = queue;
}
void QtAudioPlayer::start()
{
    m_Started = true;
    if(m_SpecSet)
    {
        actualStart();
    }
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
    //connect(m_AudioOutput, SIGNAL(notify()), SLOT(notified()));
    //connect(m_AudioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    m_AudioBuffer = m_AudioOutput->start();

    m_SpecSet = true;
    if(m_Started)
    {
        actualStart();
    }
}
void QtAudioPlayer::actualStart()
{
    m_Timer->start(20);
}
void QtAudioPlayer::fillAudioBuffer()
{
    if (m_AudioOutput && m_AudioOutput->state() != QAudio::StoppedState)
    {
        int chunks = m_AudioOutput->bytesFree()/m_AudioOutput->periodSize();
        while (chunks)
        {
           QByteArray buffer = m_Queue->deQueue(m_AudioOutput->periodSize());
           int bufferSize = buffer.size();
           if (bufferSize > 0)
               m_AudioBuffer->write(buffer, buffer.size());
           if (bufferSize != m_AudioOutput->periodSize())
               break;
           --chunks;
        }
    }
    else
    {
        m_Timer->stop();
    }
}
