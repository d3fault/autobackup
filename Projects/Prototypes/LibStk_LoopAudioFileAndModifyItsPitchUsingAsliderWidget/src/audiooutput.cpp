#include "audiooutput.h"

#include <QtMultimedia/QAudioOutput>
#include <QDebug>

#include "stkfileloopiodevice.h"

using namespace stk;

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent)
{
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate((StkFloat)44100);

    input.reset(new stk::FileLoop);
    // Try to load the soundfile.
    try
    {
        input->openFile("/run/shm/audio.wav");
    }
    catch(StkError &stkError)
    {
        //showStdStringError(stkError.getMessage());
        //close();
        qDebug() << QString::fromStdString(stkError.getMessage());
        return;
    }

    // Set input read rate based on the default STK sample rate.
    double rate = 1.0;
    rate = input->getFileRate() / Stk::sampleRate();
    //rate *= 2.0; //uncomment to play back audio at twice the speed
    input->setRate(rate);
    qDebug() << "setting input file loop rate to:" << rate;

    //input->ignoreSampleRateChange();

    // Find out how many channels we have.
    int channels = input->channelsOut();

#if 0
    QList<QAudioDeviceInfo> allDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    //qDebug() << "all devices:" << allDevices;
    for(int i = 0; i < allDevices.size(); ++i)
    {
        const QAudioDeviceInfo &currentInfo = allDevices.at(i);
        qDebug() << "device #" << i << ":" << currentInfo.deviceName();
        qDebug() << "supported sample sizes:" << currentInfo.supportedSampleSizes();
        qDebug() << "supported endiannesses:" << currentInfo.supportedByteOrders();
        qDebug() << "";
    }
#endif

    QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(Stk::sampleRate());
    audioFormat.setChannelCount(channels);
    audioFormat.setSampleSize(32);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::Float);

    if(!audioDeviceInfo.isFormatSupported(audioFormat))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        audioFormat = audioDeviceInfo.nearestFormat(audioFormat);
    }
    m_AudioOutput = new QAudioOutput(audioDeviceInfo, audioFormat, this);
    unsigned int numBufferFrames;
    tuneAudioOutputBufferSizeForLowLatency(audioFormat, &numBufferFrames);
    m_FileLoopIoDevice = new StkFileLoopIoDevice(input.take(), channels, numBufferFrames, m_AudioOutput->bufferSize(), m_AudioOutput);
}
AudioOutput::~AudioOutput()
{
    stopAudioOutput();
}
void AudioOutput::tuneAudioOutputBufferSizeForLowLatency(const QAudioFormat &audioFormat, unsigned int *out_numBufferFrames)
{
    //dynamic tuning would be neat, pushing the limits of modern hardware... but hard-coded tuning is good enough for now
    //~5ms is a good TESTING start point; I think I'll want to lower it after getting it at least working at 5ms
    qint32 numFramesFor5msDuration = audioFormat.framesForDuration(50 * 1000 /*5ms * 1000 = 5000 microseconds*/); //we don't care about rounding errors in this call, but from here on we want to use numFramesFor5msDuration as the authorative bufferSize indicator (so no more rounding errors allowed)
    qint32 bufferSizeInBytes = audioFormat.bytesForFrames(numFramesFor5msDuration);
    m_AudioOutput->setBufferSize(bufferSizeInBytes);
    *out_numBufferFrames = static_cast<unsigned int>(numFramesFor5msDuration);
}
void AudioOutput::startAudioOutput()
{
    m_FileLoopIoDevice->start();

    int desiredBufferSize = m_AudioOutput->bufferSize();
    m_AudioOutput->start(m_FileLoopIoDevice);
    int actualBufferSize = m_AudioOutput->bufferSize();
    if(desiredBufferSize != actualBufferSize)
    {
        qWarning() << "m_AudioOutput desiredBufferSize != actualBufferSize";
        qWarning() << "desiredBufferSize:" << desiredBufferSize;
        qWarning() << "actualBufferSize:" << actualBufferSize;
        qWarning() << "this means that the buffer size in m_FileLoopIoDevice (desired) is not the same as the buffer size in m_AudioOutput (actual). this may or may not affect performance";
    }
    else
    {
        qDebug() << "buffer size (bytes):" << actualBufferSize;
    }
}
void AudioOutput::stopAudioOutput()
{
    if(m_AudioOutput->state() != QAudio::StoppedState)
        m_AudioOutput->stop();
    if(m_FileLoopIoDevice->isStarted())
        m_FileLoopIoDevice->stop();
}
void AudioOutput::changePitchShift(stk::StkFloat newPitchShiftValue)
{
    m_FileLoopIoDevice->setPitchShift(newPitchShiftValue);
}
void AudioOutput::changePitchShiftMixAmount(stk::StkFloat newPitchShiftMixAmount)
{
    m_FileLoopIoDevice->setPitchShiftMixAmount(newPitchShiftMixAmount);
}
