#include "musicfingersserialportintegration.h"

#include <QStringList>

//Maybe I should say fuck the synth for now and hook this shit up to a/the samples piano. Only downside is that the analog values would be mostly wasted, HOWEVER here's one use of them: you pass a threshold with your finger to start the piano sample at a minimum volume, and as the finger becomes more curled, the volume of the sample increases. That should be an optional mode, the default being binary/boolean on/off (playing the sample at a constant volume once you pass the threshold). Of course the samples are pretty short (but also, the speed of light is pretty quick), so I'm not sure how noticeable it would be. REGARDLESS, it will be a step in proper modulization of the fingers+serialport stuff, which should be portable to keyboard/mouse shit too (but OTOH, the best way to do that is with a leonardo/micro, and my micro is in the mail -- I'm not sure if I should use Serial.write(key) and Keyboard.write(key) simultaneously -- one for logging/key-set-switching/mode-switching/etc and the other to be interpreted by the OS). A vanilla piano mode is desireable. And even though my sin-synth is utter garbage, it does provide a "target" for implementing/testing some kind of "mode switching". I think the music modes should all be mutually exclusive with each other, and keyboard and mouse should be mutually exclusive with each other, but ANY COMBINATION of "any music mode" and "either keyboard or mouse or neither" should be allowed.
//A vanilla piano is also a much more tangible target, this synth shit needs a TON of work (AND HEY, I WANT TO GO THE FUCK OUTSIDE)
//TODOoptional: I only have 10 fingers, so that means only 10 vanilla piano keys until I implement some sort of switching (but I'm fine with 10 on the short term anyways). I realized I have 10x unused potentiometers laying around... I could use just one of them to slide left and right on the piano... or I guess 2 of them (one for each hand's "position" on the "piano"). Could mount those fuckers on my wrists or wherever really, I don't need to mess with them THAT often. *spends the rest of his life tapping his wrists over and over* (nah jk it's a short term and easy as fuck solution -- i want some kind of gestures shit but idk fuck it I'll make that up later)
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
bool MusicFingersSerialPortIntegration::serialPortSelectionIsValid(const QString &serialPortSelection, int numSerialPorts)
{
    int enteredNum = -1;
    bool convertOk = false;
    enteredNum = serialPortSelection.toInt(&convertOk);
    if(!convertOk)
    {
        //emit e("invalid input. enter a number");
        return false;
    }
    if(enteredNum < 0 || enteredNum > (numSerialPorts-1))
    {
        //emit e("invalid input. no serial port at that index");
        return false;
    }
    return true;
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
    if(!Finger::isValidFingerId(fingerId))
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
    emit fingerMoved(Finger::fingerIdToFingerEnum(fingerId), fingerPosition);
}
void MusicFingersSerialPortIntegration::handleSerialPortReadyRead()
{
    while(canReadLine())
    {
        processLineOfFingerMovementProtocol(readLine().trimmed());
    }
}
