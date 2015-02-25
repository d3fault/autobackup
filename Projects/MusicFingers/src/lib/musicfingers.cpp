#include "musicfingers.h"

#include <QtMultimedia/QAudioOutput>

#include "math.h"
#include <QDebug>

#define FINGERS_INITIAL_POSITION ((MUSIC_FINGERS_MAX_POSITION/2)+1) //Divided by two to put it "in the middle", plus one to put it at "key NOT pressed"

MusicFingers::MusicFingers(QObject *parent)
    : QObject(parent)
    , m_TimeInBytesForMySineWave(0) //TODOreq: every "sine wave 'period' (when the y value is the same, leik at the top or wherever really (traditionally the top))", I need to put this back at zero. Otherwise, when we run out of range in the integer, there might be a small popping sound
{
    m_Fingers.insert(Finger::LeftPinky0, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftRing1, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftMiddl2, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftIndex3, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftThumb4, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightThumb5, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightIndex6, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightMiddle, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightRing7, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightPinky8, FINGERS_INITIAL_POSITION);

    //QAudioDeviceInfo audioDeviceInfo;
    QAudioFormat audioFormat/* = audioDeviceInfo.preferredFormat()*/; //TODOreq: synthesizer must also generate same size. TODOmb: 10 channels instead of 1 or 2? probably not, since we're "muxing" the 10 fingers into 1 audio stream... but hey I mean you can always experiment with 10 channels, idfk -- it would be like 10x surround sound speakers ish
    audioFormat.setByteOrder(QAudioFormat::BigEndian);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleRate(48000);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::Float);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug() << "Using audio device: " << info.deviceName();
    if(!info.isFormatSupported(audioFormat))
    {
        qWarning() << "Audio format not supported by backend, cannot play audio.";
        qWarning() << "supportedByteOrders" << info.supportedByteOrders();
        qWarning() << "supportedCodecs" << info.supportedCodecs();
        qWarning() << "supportedSampleTypes" << info.supportedSampleTypes();
        qWarning() << "supportedChannelCounts" << info.supportedChannelCounts();
        qWarning() << "supportedSampleRates" << info.supportedSampleRates();
        qWarning() << "supportedSampleSizes" << info.supportedSampleSizes();
        return;
    }
    m_AudioOutput = new QAudioOutput(audioFormat, this);
    connect(m_AudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioOutputStateChanged(QAudio::State)));
    connect(m_AudioOutput, SIGNAL(notify()), this, SLOT(handleAudioOutputNotifiedMeItWantsData()));
    m_AudioOutputIoDevice = m_AudioOutput->start();
    QAudio::Error error = m_AudioOutput->error();
    if(error != QAudio::NoError)
    {
        qWarning() << "Audio output has error:" << error;
    }
}
QByteArray MusicFingers::synthesizeAudioUsingFingerPositions(int numBytesToSynthesize_aka_audioOutputPeriodSize)
{
    QByteArray ret; //TODOoptimization: maybe smart to re-use this buffer, maybe doesn't matter
    ret.reserve(numBytesToSynthesize_aka_audioOutputPeriodSize);
    static const int charsInDouble = sizeof(double) / sizeof(char);
    for(int i = 0; i < numBytesToSynthesize_aka_audioOutputPeriodSize; ++i)
    {
        char hackyEightDoubles[charsInDouble];
        for(int j = 0; j < charsInDouble; ++j)
        {
            hackyEightDoubles[j] = m_TimeInBytesForMySineWave;
            ++m_TimeInBytesForMySineWave;
        }
        double *lel = (double*)(&hackyEightDoubles);
        ret.insert(i, sin(*lel));
    }
    return ret;
}
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    qDebug() << fingerToMove << newPosition;
    m_Fingers.insert(fingerToMove, newPosition);
}
void MusicFingers::handleAudioOutputStateChanged(QAudio::State newState)
{
    qDebug() << "Audio Output changed state to: " << newState;
}
void MusicFingers::handleAudioOutputNotifiedMeItWantsData()
{
    qDebug() << "yolo";
    m_AudioOutputIoDevice->write(synthesizeAudioUsingFingerPositions(m_AudioOutput->periodSize()));
}
