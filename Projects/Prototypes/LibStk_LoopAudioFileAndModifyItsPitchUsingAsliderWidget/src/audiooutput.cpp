#include "audiooutput.h"

#include <QtMultimedia/QAudioOutput>
#include <QDebug>

#include "stkfileloopiodevice.h"

using namespace stk;

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent)
    , m_NumBufferFrames(RT_BUFFER_SIZE)
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
    m_FileLoopIoDevice = new StkFileLoopIoDevice(input.take(), channels, m_AudioOutput);
    m_FileLoopIoDevice->reserveFrames(m_NumBufferFrames, channels);
}
void AudioOutput::startAudioOutput()
{
    m_FileLoopIoDevice->start();
    m_AudioOutput->start(m_FileLoopIoDevice);
}
void AudioOutput::stopAudioOutput()
{
    m_AudioOutput->stop();
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
