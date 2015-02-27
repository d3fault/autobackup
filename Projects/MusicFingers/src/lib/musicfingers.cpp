#include "musicfingers.h"

#include <QtMultimedia/QAudioOutput>

#include "math.h"
#include <QDebug>
#include <QTimer>
#include <QtSerialPort/QSerialPortInfo>

#define MusicFingers_NUM_PERIODS_TO_WRITE_PER_TIMEOUT 1
//#define MusicFingers_

//TODOmb: if all of [5] (one hand) of the finger sensors move at the same time (and the same distance), it means the WRIST is moving, not [necessarily] the fingers.... so I can/should filter that wrist movement out. Differentiating between that and intentional movement of all 5 fingers would be difficult, if not impossible
MusicFingers::MusicFingers(QObject *parent)
    : QObject(parent)
    , m_TimeInBytesForMySineWave(-1.0) //TODOreq: every "sine wave 'period' (when the y value is the same, leik at the top or wherever really (traditionally the top))", I need to put this back at zero. Otherwise, when we run out of range in the integer, there might be a small popping sound
{
    QList<QSerialPortInfo> availPorts = QSerialPortInfo::availablePorts();
    if(availPorts.isEmpty())
    {
        qWarning() << "No serial ports available";
        return;
    }

    m_SerialPort.reset(new QSerialPort(availPorts.first(), this)); //TODOoptional: dialog/etc when more than one available port
    m_SerialPort->setBaudRate(QSerialPort::Baud9600);
    if(!m_SerialPort->open(QIODevice::ReadOnly))
    {
        emit e("failed to open serial port: " + QSerialPortInfo::availablePorts().first().description());
        emit quitRequested();
        return;
    }
    connect(m_SerialPort.data(), SIGNAL(readyRead()), this, SLOT(handleSerialPortReadyRead()));

    m_Fingers.insert(Finger::LeftPinky_A0, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftRing_A1, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftMiddle_A2, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftIndex_A3, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::LeftThumb_A4, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightThumb_A5, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightIndex_A6, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightMiddle_A7, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightRing_A8, FINGERS_INITIAL_POSITION);
    m_Fingers.insert(Finger::RightPinky_A9, FINGERS_INITIAL_POSITION);

    //QAudioDeviceInfo audioDeviceInfo;
    QAudioFormat audioFormat/* = audioDeviceInfo.preferredFormat()*/; //TODOreq: synthesizer must also generate same size. TODOmb: 10 channels instead of 1 or 2? probably not, since we're "muxing" the 10 fingers into 1 audio stream... but hey I mean you can always experiment with 10 channels, idfk -- it would be like 10x surround sound speakers ish
    audioFormat.setByteOrder(QAudioFormat::BigEndian);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleRate(48000);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::UnSignedInt);
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
    m_AudioOutput = new QAudioOutput(audioFormat, this);
    connect(m_AudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioOutputStateChanged(QAudio::State)));
    //connect(m_AudioOutput, SIGNAL(notify()), this, SLOT(writeBytesToAudioOutput()));
    QTimer *writeAudioTimer = new QTimer(this);
    connect(writeAudioTimer, SIGNAL(timeout()), this, SLOT(writeBytesToAudioOutput()));
    m_AudioOutputIoDevice = m_AudioOutput->start();
    QAudio::Error error = m_AudioOutput->error();
    if(error != QAudio::NoError)
    {
        qWarning() << "Audio output has error:" << error;
        return;
    }
    writeAudioTimer->start(2);
}
bool MusicFingers::isValidFingerId(int fingerId)
{
    return (fingerId > -1 && fingerId < 10);
}
QByteArray MusicFingers::synthesizeAudioUsingFingerPositions(int numBytesToSynthesize_aka_audioOutputPeriodSize)
{
    //TODOoptional: runtime math formula random permuations (press space key = new random math formula (it tells you which ofc (and ideally all are recorded (and ideally rated based on how i like em))))

    QByteArray ret; //TODOoptimization: maybe smart to re-use this buffer, maybe doesn't matter
    ret.reserve(numBytesToSynthesize_aka_audioOutputPeriodSize);
    static const int numberOfDoublesWeCanFitIntoRet = (numBytesToSynthesize_aka_audioOutputPeriodSize*sizeof(char)) / sizeof(double);
    //for(int i = 0; i < (numberOfDoublesWeCanFitIntoRet/2); ++i)
    for(int i = 0; i < numberOfDoublesWeCanFitIntoRet; ++i)
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

#if 1
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

        ret.append(y);
    }
    return ret;
}
void MusicFingers::processLineOfFingerMovementProtocol(const QString &lineOfFingerMovementProtocol)
{
    QStringList lineParts = lineOfFingerMovementProtocol.split(":");
    if(lineParts.size() != 2)
    {
        emit e("processLineOfFingerMovementProtocol line parts did not split into 2. instead: " + lineParts.size());
        emit e("line: " + lineOfFingerMovementProtocol);
        return;
    }

    bool convertOk = false;
    int fingerId = lineParts.at(0).toInt(&convertOk);
    if(!convertOk)
    {
        emit e("failed to convert first part (left side of colon, the finger id) of the lineOfFingerMovementProtocol");
        emit e("line: " + lineOfFingerMovementProtocol);
        return;
    }
    if(!isValidFingerId(fingerId))
    {
        emit e("invalid finger id in first part (left side of colon) of the lineOfFingerMovementProtocol");
        emit e("line: " + lineOfFingerMovementProtocol);
        return;
    }

    convertOk = false;
    int fingerPosition = lineParts.at(1).toInt(&convertOk);
    if(!convertOk)
    {
        emit e("failed to convert second part (right side of colon, the finger position value) of the lineOfFingerMovementProtocol");
        emit e("line: " + lineOfFingerMovementProtocol);
        return;
    }

    if(fingerPosition < MUSIC_FINGERS_MIN_POSITION || fingerPosition > MUSIC_FINGERS_MAX_POSITION)
    {
        emit e("finger position was out of range: " + QString::number(fingerPosition));
        emit e("line: " + lineOfFingerMovementProtocol);
        return;
    }
    m_Fingers.insert(fingerIdToFingerEnum(fingerId), fingerPosition);
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
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    //qDebug() << fingerToMove << newPosition;
    m_Fingers.insert(fingerToMove, newPosition);
}
void MusicFingers::queryMusicFingerPosition(int fingerIndex)
{
    emit o("Finger position: " + QString::number(m_Fingers.value(fingerIdToFingerEnum(fingerIndex))));
}
void MusicFingers::handleAudioOutputStateChanged(QAudio::State newState)
{
    qDebug() << "Audio Output changed state to: " << newState;
}
void MusicFingers::writeBytesToAudioOutput()
{
    m_AudioOutputIoDevice->write(synthesizeAudioUsingFingerPositions(m_AudioOutput->periodSize()*MusicFingers_NUM_PERIODS_TO_WRITE_PER_TIMEOUT));
}
void MusicFingers::handleSerialPortReadyRead()
{
    while(m_SerialPort->canReadLine())
    {
        processLineOfFingerMovementProtocol(m_SerialPort->readLine().trimmed());
    }
}
