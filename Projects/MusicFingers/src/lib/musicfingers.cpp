#include "musicfingers.h"

#include <QtMultimedia/QAudioOutput>

#include <QDebug>

//TODOoptional: obvioiusly, [re-]synthing the finger positions into audio during the time of the "copy mux" (the one that is placed on the website for viewing) should be done (rather than lossy-as-FUCK 'recording' of the 'speakers' being the only expression of that audio)
//TODOmb: if all of [5] (one hand) of the finger sensors move at the same time (and the same distance), it means the WRIST is moving, not [necessarily] the fingers.... so I can/should filter that wrist movement out. Differentiating between that and intentional movement of all 5 fingers would be difficult, if not impossible
//TODOoptional: easing curves (both for the 'linear transition, but also optionally for overshooting and doubling back all sexy like) when moving the finger. OutElastic seems best (from the visual example I'm testing), but OutBack seems aight too. I think using the QAnimation classes directly might help, but am unsure. I need to make sure that I "adjust" the animated easing curve, instead of "restarting" it for every finger movement (obviously once the animation/easing-curve is OVER, then I "restart" it on the next finger movement). When starting this to do, I thought it would just be a neat experiment, now I'm pretty sure QPropertyAnimation is the proper solution to the 'linear transition' problem (choppiness) :-P. I'm going to make an assumption and say the Qt devs are fucking smart as fuck and probably handle changing the destination/end value of the property animation while the animation is running...
MusicFingers::MusicFingers(QObject *parent)
    : QObject(parent)
    , m_AudioOutputHasBeenIdleBefore(false)
    , m_NumPeriodsToWriteEachTime(1)
    //, m_TimeInBytesForMySineWave(-1.0) //TODOreq: every "sine wave 'period' (when the y value is the same, leik at the top or wherever really (traditionally the top))", I need to put this back at zero. Otherwise, when we run out of range in the integer, there might be a small popping sound
{
    //QAudioDeviceInfo audioDeviceInfo;
    QAudioFormat audioFormat/* = audioDeviceInfo.preferredFormat()*/; //TODOreq: synthesizer must also generate same size. TODOmb: 10 channels instead of 1 or 2? probably not, since we're "muxing" the 10 fingers into 1 audio stream... but hey I mean you can always experiment with 10 channels, idfk -- it would be like 10x surround sound speakers ish
    audioFormat.setChannelCount(1);
    audioFormat.setSampleRate(MUSIC_FINGERS_SAMPLE_RATE);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug() << "Using audio device: " << info.deviceName();
    if(!info.isFormatSupported(audioFormat))
    {
        qWarning() << "Audio format not supported by backend, using nearest format.";
        qWarning() << "supportedByteOrders" << info.supportedByteOrders();
        qWarning() << "supportedCodecs" << info.supportedCodecs();
        qWarning() << "supportedSampleTypes" << info.supportedSampleTypes();
        qWarning() << "supportedChannelCounts" << info.supportedChannelCounts();
        qWarning() << "supportedSampleRates" << info.supportedSampleRates();
        qWarning() << "supportedSampleSizes" << info.supportedSampleSizes();

        qWarning() << "";
        audioFormat = info.nearestFormat(audioFormat);
        qWarning() << "Using audio format: " << audioFormat;
        return;
    }
    //m_SampleRate = audioFormat.sampleRate();
    m_AudioOutput = new QAudioOutput(audioFormat, this);
    m_MusicFingersSynthesizer = new MusicFingersSynthesizer(m_AudioOutput);
    if(!m_MusicFingersSynthesizer->open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open MusicFingersSynthesizer (a QIODevice)";
        return;
    }
    connect(m_AudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioOutputStateChanged(QAudio::State)));
    m_AudioOutput->start(m_MusicFingersSynthesizer);
    m_MusicFingersSynthesizer->setLengthOfDataToSynthesizeWhenReadDataIsCalled(m_AudioOutput->periodSize() * m_NumPeriodsToWriteEachTime);
    QAudio::Error error = m_AudioOutput->error();
    if(error != QAudio::NoError)
    {
        qWarning() << "Audio output has error:" << error;
        return;
    }
}
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    //qDebug() << fingerToMove << newPosition;
    m_MusicFingersSynthesizer->moveFinger(fingerToMove, newPosition);
}
void MusicFingers::queryMusicFingerPosition(int fingerIndex)
{
    emit o("Finger position: " + QString::number(m_MusicFingersSynthesizer->m_Fingers.value(Finger::fingerIdToFingerEnum(fingerIndex))->position()));
}
void MusicFingers::handleAudioOutputStateChanged(QAudio::State newState)
{
    if(newState == QAudio::IdleState)
    {
        if(m_AudioOutputHasBeenIdleBefore) //this state is set after start() is called, so we ignore it once. TODOmb: if we ever do stop/restart, we should set this back to false
        {
            m_NumPeriodsToWriteEachTime *= 2;
            //m_MusicFingersSynthesizer.setTargetSynthSize
            m_MusicFingersSynthesizer->setLengthOfDataToSynthesizeWhenReadDataIsCalled(m_AudioOutput->periodSize() * m_NumPeriodsToWriteEachTime);
        }
        m_AudioOutputHasBeenIdleBefore = true;
    }
    qDebug() << "Audio Output changed state to: " << newState;
}
