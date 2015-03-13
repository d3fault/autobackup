#include "musicfingersserialportintegration.h"

#include <QStringList>

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
