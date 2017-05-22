#ifndef WASDFARDUINO_H
#define WASDFARDUINO_H

#include <QObject>

#include <QTextStream>

#include "wasdf.h"

class QSerialPort;

class WasdfArduino : public QObject
{
    Q_OBJECT
public:
    explicit WasdfArduino(QObject *parent = 0);
private:
    void openSerialPortIfNotOpen();
    void sendCommandToArduino(const QString &command);

    QSerialPort *m_SerialPort;
    QTextStream m_SerialPortTextStream;
signals:
    void e(const QString &msg);
    void analogPinReadingChangedDuringCalibration(int analogPinId, int newPosition);
    void fingerMoved(Finger finger, int newPosition);
public slots:
    void startInCalibrationMode();
    void start(const WasdfCalibrationConfiguration &calibrationConfig);
private slots:
    void handleSerialPortReadyReadCalibrationMode();
    void handleSerialPortReadyReadNormalFingerMovementMode();
};

#endif // WASDFARDUINO_H
