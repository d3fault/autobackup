#include "musicfingers.h"

#include <QtMultimedia/QAudioOutput>

#include "math.h"
#include <QDebug>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define MusicFingers_NUM_PERIODS_TO_WRITE_PER_TIMEOUT 1

MusicFingers::MusicFingers(QObject *parent)
    : QObject(parent)
    , m_SerialPort(new QSerialPort(QSerialPortInfo::availablePorts().first(), this)) //TODOreq: dialog when more than one available port
    , m_TimeInBytesForMySineWave(-1.0) //TODOreq: every "sine wave 'period' (when the y value is the same, leik at the top or wherever really (traditionally the top))", I need to put this back at zero. Otherwise, when we run out of range in the integer, there might be a small popping sound
{
    m_SerialPort->setBaudRate(QSerialPort::Baud9600);
    if(!m_SerialPort->open(QIODevice::ReadOnly))
    {
        emit e("failed to open serial port: " + QSerialPortInfo::availablePorts().first().description());
        emit quitRequested();
        return;
    }
    connect(m_SerialPort, SIGNAL(readyRead()), this, SLOT(handleSerialPortReadyRead()));

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
QByteArray MusicFingers::synthesizeAudioUsingFingerPositions(int numBytesToSynthesize_aka_audioOutputPeriodSize)
{
    QByteArray ret; //TODOoptimization: maybe smart to re-use this buffer, maybe doesn't matter
    ret.reserve(numBytesToSynthesize_aka_audioOutputPeriodSize);
    static const int numberOfDoublesWeCanFitIntoRet = (numBytesToSynthesize_aka_audioOutputPeriodSize*sizeof(char)) / sizeof(double);
    for(int i = 0; i < numberOfDoublesWeCanFitIntoRet; ++i)
    {
        double y = sin(++m_TimeInBytesForMySineWave)*m_Fingers.value(Finger::LeftPinky0);
        //double y = pow(sin(++m_TimeInBytesForMySineWave), static_cast<double>(m_Fingers.value(Finger::LeftPinky0)));

        ret.insert(i, y);
        //qDebug() << y;
    }
    return ret;
}
void MusicFingers::moveFinger(Finger::FingerEnum fingerToMove, int newPosition)
{
    //qDebug() << fingerToMove << newPosition;
    m_Fingers.insert(fingerToMove, newPosition);
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
    int latestValue = -1;
    while(m_SerialPort->canReadLine())
    {
        QByteArray line = m_SerialPort->readLine();
        bool convertOk = false;
        int value = line.trimmed().toInt(&convertOk);
        if(convertOk && value >= MUSIC_FINGERS_MIN_POSITION && value <= MUSIC_FINGERS_MAX_POSITION)
        {
            latestValue = value;
        }
    }
    if(latestValue > -1)
    {
        m_Fingers.insert(Finger::LeftPinky0, latestValue);
    }
}
