#ifndef MUSICFINGERSSERIALPORTINTEGRATION_H
#define MUSICFINGERSSERIALPORTINTEGRATION_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

#include "musicfingers.h"

#define CHOOSE_MUSIC_FINGERS_SERIAL_PORT_INFO_OR_ELSEANDRETURN(serialPortInfoVarName, orElse) \
QList<QSerialPortInfo> availPorts = QSerialPortInfo::availablePorts(); \
QSerialPortInfo serialPortInfoVarName; \
if(availPorts.isEmpty()) \
{ \
    qWarning() << "No serial ports available"; \
    orElse; \
    return; \
} \
else if(availPorts.size() > 1) \
{ \
    handleO("Choose your serial port (empty line or 'q' exits):"); \
    handleO(""); \
    int currentPortIndex = 0; \
    Q_FOREACH(const QSerialPortInfo &currentPort, availPorts) \
    { \
        handleO("\t" + QString::number(currentPortIndex) + " - " + currentPort.portName() + " (" + currentPort.description() + " (" + currentPort.systemLocation() + "))"); \
        ++currentPortIndex; \
    } \
    handleO(""); \
    QTextStream stdIn(stdin); \
    QString line; \
    do \
    { \
        line = stdIn.readLine(); \
        if(line.isNull() || line.isEmpty() || line.toLower().startsWith("q")) \
        { \
            orElse; \
            return; \
        } \
    }while(!MusicFingersSerialPortIntegration::serialPortSelectionIsValid(line, availPorts.size())); \
    int chosenSerialPortIndex = line.toInt(); /*already sanitized, will convert*/ \
    serialPortInfoVarName = availPorts.at(chosenSerialPortIndex); \
} \
else /*exactly 1 serial port*/ \
{ \
    serialPortInfoVarName = availPorts.at(0); \
}

class MusicFingersSerialPortIntegration : public QSerialPort
{
    Q_OBJECT
public:
    explicit MusicFingersSerialPortIntegration(QObject *parent = Q_NULLPTR);
    explicit MusicFingersSerialPortIntegration(const QString &name, QObject *parent = Q_NULLPTR);
    explicit MusicFingersSerialPortIntegration(const QSerialPortInfo &info, QObject *parent = Q_NULLPTR);
    static bool serialPortSelectionIsValid(const QString &serialPortSelection, int numSerialPorts);
    virtual ~MusicFingersSerialPortIntegration();
private:
    void privateConstructor();
    void processLineOfFingerMovementProtocol(const QString &lineOfFingerMovementProtocol);
signals:
    void e(const QString &msg);
    void fingerMoved(Finger::FingerEnum finger, int newPosition);
private slots:
    void handleSerialPortReadyRead();
};

#endif // MUSICFINGERSSERIALPORTINTEGRATION_H
