#include "musicfingersserialportintegration.h"

#include <QStringList>

//Maybe I should say fuck the synth for now and hook this shit up to a/the samples piano. Only downside is that the analog values would be mostly wasted, HOWEVER here's one use of them: you pass a threshold with your finger to start the piano sample at a minimum volume, and as the finger becomes more curled, the volume of the sample increases. That should be an optional mode, the default being binary/boolean on/off (playing the sample at a constant volume once you pass the threshold). Of course the samples are pretty short (but also, the speed of light is pretty quick), so I'm not sure how noticeable it would be. REGARDLESS, it will be a step in proper modulization of the fingers+serialport stuff, which should be portable to keyboard/mouse shit too (but OTOH, the best way to do that is with a leonardo/micro, and my micro is in the mail -- I'm not sure if I should use Serial.write(key) and Keyboard.write(key) simultaneously -- one for logging/key-set-switching/mode-switching/etc and the other to be interpreted by the OS). A vanilla piano mode is desireable. And even though my sin-synth is utter garbage, it does provide a "target" for implementing/testing some kind of "mode switching". I think the music modes should all be mutually exclusive with each other, and keyboard and mouse should be mutually exclusive with each other, but ANY COMBINATION of "any music mode" and "either keyboard or mouse or neither" should be allowed.
//A vanilla piano is also a much more tangible target, this synth shit needs a TON of work (AND HEY, I WANT TO GO THE FUCK OUTSIDE)
MusicFingersSerialPortIntegration::MusicFingersSerialPortIntegration(QObject *parent)
    : QSerialPort(parent)
{
    privateConstructor();
}
MusicFingersSerialPortIntegration::MusicFingersSerialPortIntegration(const QString &name, QObject *parent)
    : QSerialPort(name, parent)
{
    privateConstructor();
}
MusicFingersSerialPortIntegration::MusicFingersSerialPortIntegration(const QSerialPortInfo &info, QObject *parent)
    : QSerialPort(info, parent)
{
    privateConstructor();
}
MusicFingersSerialPortIntegration::~MusicFingersSerialPortIntegration()
{ }
void MusicFingersSerialPortIntegration::privateConstructor()
{
    setBaudRate(QSerialPort::Baud9600); //TODOoptional: QSerialPortInfo supplies a list of "standard baud rates" that the serial port supports
    connect(this, SIGNAL(readyRead()), this, SLOT(handleSerialPortReadyRead()));
}
void MusicFingersSerialPortIntegration::processLineOfFingerMovementProtocol(const QString &lineOfFingerMovementProtocol)
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
    if(!MusicFingers::isValidFingerId(fingerId))
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
    emit fingerMoved(MusicFingers::fingerIdToFingerEnum(fingerId), fingerPosition);
}
void MusicFingersSerialPortIntegration::handleSerialPortReadyRead()
{
    while(canReadLine())
    {
        processLineOfFingerMovementProtocol(readLine().trimmed());
    }
}
