#include "qtaudioplayer.h"

QtAudioPlayer::QtAudioPlayer(QObject *parent) :
    QObject(parent), m_Started(false), m_SpecSet(false)
{
}
void QtAudioPlayer::init()
{
    //nothing to do here because we need an audio spec!
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
void QtAudioPlayer::handleNewAudioBytes(QByteArray newAudioBytes)
{
    if (m_AudioOutput && m_AudioOutput->state() != QAudio::StoppedState)
    {
        int chunks = newAudioBytes.size()/m_AudioOutput->periodSize();
        while (chunks)
        {
           QByteArray buffer = newAudioBytes.left(m_AudioOutput->periodSize());
           int bufferSize = buffer.size();
           if(bufferSize < newAudioBytes.size())
               newAudioBytes.remove(0, bufferSize);
           if (bufferSize > 0)
               m_AudioBuffer->write(buffer, buffer.size());
           if (bufferSize != m_AudioOutput->periodSize())
           {
               break;
           }
           --chunks;
        }
    }
    else
    {
        m_Timer->stop();
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
    m_Timer->start(20);
}
void QtAudioPlayer::fillAudioBuffer()
{
    //TODO: potential race condition if synchronizer doesn't respond within 20ms... which is pretty damn long so i doubt would ever happen. still, the theoretical potential exists. boolean "dataHasBeenRequested" = true; here and checked every time would fix it. set it to false in handleNewAudioBytes
    if (m_AudioOutput && m_AudioOutput->state() != QAudio::StoppedState)
    {
        emit needAudio(m_AudioOutput->bytesFree());
    }
    else
    {
        m_Timer->stop();
    }
}
