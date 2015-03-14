#include "musicfingers.h"

#include <QtMultimedia/QAudioOutput>

#include <QDebug>

//TODOmb: if all of [5] (one hand) of the finger sensors move at the same time (and the same distance), it means the WRIST is moving, not [necessarily] the fingers.... so I can/should filter that wrist movement out. Differentiating between that and intentional movement of all 5 fingers would be difficult, if not impossible
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
bool MusicFingers::isValidFingerId(int fingerId)
{
    return (fingerId > -1 && fingerId < 10);
}
Finger::FingerEnum MusicFingers::fingerIdToFingerEnum(int fingerId)
{
    switch(fingerId)
    {
    case 0:
        return Finger::A0_LeftPinky;
    case 1:
        return Finger::A1_LeftRing;
    case 2:
        return Finger::A2_LeftMiddle;
    case 3:
        return Finger::A3_LeftIndex;
    case 4:
        return Finger::A4_LeftThumb;
    case 5:
        return Finger::A5_RightThumb;
    case 6:
        return Finger::A6_RightIndex;
    case 7:
        return Finger::A7_RightMiddle;
    case 8:
        return Finger::A8_RightRing;
    case 9:
        return Finger::A9_RightPinky;
    default:
        return Finger::A0_LeftPinky; //should never happen, caller should (and does atm) sanitize before calling
    }
}
#if 0 //isuck
QByteArray MusicFingers::synthesizeAudioUsingFingerPositions(int numBytesToSynthesize)
{
    //TODOoptional: runtime math formula random permuations (press space key = new random math formula (it tells you which ofc (and ideally all are recorded (and ideally rated based on how i like em))))

    QByteArray ret; //TODOoptimization: maybe smart to re-use this buffer, maybe doesn't matter
    ret.reserve(numBytesToSynthesize);
    //ret.resize(numBytesToSynthesize);
    int numberOfShortsWeCanFitIntoRet = numBytesToSynthesize / sizeof(short);
    //for(int i = 0; i < (numberOfDoublesWeCanFitIntoRet/2); ++i)
    for(int i = 0; i < numberOfShortsWeCanFitIntoRet; ++i)
    {
        int a0_fingerPos = m_Fingers.value(Finger::LeftPinky_A0);
        int a1_fingerPos = m_Fingers.value(Finger::A1_LeftRing);


        //double y = sin(++m_TimeInBytesForMySineWave)*fingerPos;

        //mute
        //double y = pow(sin(++m_TimeInBytesForMySineWave), static_cast<double>(fingerPos));

#if 0
        //YES
        double y = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;
#endif

        //mute
        //double y = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos);

        //mute
        //double y = sin((++m_TimeInBytesForMySineWave)/a0_fingerPos)/a0_fingerPos;

        //mute
        //double y = pow(sin((++m_TimeInBytesForMySineWave)*a0_fingerPos*a0_fingerPos), a0_fingerPos*a0_fingerPos);

        //mute
        //double y = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*pow(a0_fingerPos, a0_fingerPos);

#if 0
        //BETTER YES
        //seems the same as sin, but "faster". has cool/unique (compared to rest) sound when position is at/near: 639
        double y = tan((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;
#endif

        //mute
        //double y = asin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;

#if 0
        //soft fuzzy sound only when moving finger
        double y = asinh((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;
#endif

        //A1 added

        //a1 has little effect
        //double y = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a1_fingerPos;

        //white noise unless one of the sensors is near 1 (which makes it equivalent to my "YES" formula)
        //double y_a0 = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;
        //double y_a1 = sin((++m_TimeInBytesForMySineWave)*a1_fingerPos)*a1_fingerPos;
        //double y = y_a0 * y_a1;

        //ret.insert(i, y);

        //a1 doesn't change anything, a0 behaves like "YES"
        //double y0 = sin((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a0_fingerPos;
        //double y1 = sin((++m_TimeInBytesForMySineWave)*a1_fingerPos)*a1_fingerPos;
        //ret.append(y0);
        //ret.append(y1);

        //a1 has little effect
        //double y = tan((++m_TimeInBytesForMySineWave)*a0_fingerPos)*a1_fingerPos;

        //quiet af, movement has very little (if any) effect
        //double y = tan((++m_TimeInBytesForMySineWave*a0_fingerPos))*sin(m_TimeInBytesForMySineWave*a1_fingerPos);

        //white noise
        //double y = (tan((++m_TimeInBytesForMySineWave*a0_fingerPos))*sin(m_TimeInBytesForMySineWave*a1_fingerPos)) * (a0_fingerPos*a1_fingerPos);

        //quiet af, movement has very little (if any) effect
        //double y = tan((++m_TimeInBytesForMySineWave*a0_fingerPos)) / sin(m_TimeInBytesForMySineWave*a1_fingerPos);

        //double y = sin((2 * M_PI * (++m_TimeInBytesForMySineWave))*a0_fingerPos)*a0_fingerPos;

        //ret.append(y);

        static const long minFreq = 0;
        static const long maxFreq = 1023;
        //map 0-1023 -> minFreq-maxFreq
        long a0_fingerPos_AsLong = static_cast<long>(a0_fingerPos);
        long a0_mappedFrequency = (a0_fingerPos_AsLong - MUSIC_FINGERS_MIN_POSITION) * (maxFreq - minFreq) / (MUSIC_FINGERS_MAX_POSITION - MUSIC_FINGERS_MIN_POSITION) + minFreq; //jacked from Arduino/WMath.cpp , which in turn was jacked from Wiring. it's just math anyways (not copyrighteable)
#if 0
        if(m_TimeInBytesForMySineWave % 50000 == 0)
            emit o("Freq: " + QString::number(a0_mappedFrequency));
#endif
        //a0_mappedFrequency = 440;

        //short sample = 32760 * sin( (2.f*static_cast<float>(M_PI)*static_cast<float>(a0_mappedFrequency)) / m_SampleRate * (++m_TimeInBytesForMySineWave));

        qreal sample = qSin( (static_cast<qreal>(2) * static_cast<qreal>(M_PI) * static_cast<qreal>(a0_mappedFrequency)) / m_SampleRate * (++m_TimeInBytesForMySineWave));
        short sampleShort = static_cast<short>(sample * 32760);

        if(m_TimeInBytesForMySineWave % 100000 == 0)
            emit o("Sample: " + QString::number(sample));

        ret.append(sampleShort);
    }
    return ret;
}
#endif
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    //qDebug() << fingerToMove << newPosition;
    //m_Fingers.insert(fingerToMove, newPosition);
    m_MusicFingersSynthesizer->moveFinger(fingerToMove, newPosition);
}
void MusicFingers::queryMusicFingerPosition(int fingerIndex)
{
    emit o("Finger position: " + QString::number(m_MusicFingersSynthesizer->m_Fingers.value(fingerIdToFingerEnum(fingerIndex))));
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
#if 0
void MusicFingers::writeBytesToAudioOutput()
{
    m_AudioOutputIoDevice->write(synthesizeAudioUsingFingerPositions(m_AudioOutput->periodSize()*m_NumPeriodsToWritePerTimeout));
}
#endif
