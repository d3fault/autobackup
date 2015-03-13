#ifndef MUSICFINGERSSERIALPORTINTEGRATION_H
#define MUSICFINGERSSERIALPORTINTEGRATION_H

#include <QtSerialPort/QSerialPort>

#include "musicfingers.h"

class MusicFingersSerialPortIntegration : public QSerialPort
{
    Q_OBJECT
public:
    explicit MusicFingersSerialPortIntegration(QObject *parent = Q_NULLPTR);
    explicit MusicFingersSerialPortIntegration(const QString &name, QObject *parent = Q_NULLPTR);
    explicit MusicFingersSerialPortIntegration(const QSerialPortInfo &info, QObject *parent = Q_NULLPTR);
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
