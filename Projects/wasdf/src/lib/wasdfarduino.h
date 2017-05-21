#ifndef WASDFARDUINO_H
#define WASDFARDUINO_H

#include "wasdf.h"

class QSerialPort;

class WasdfArduino : public QObject
{
    Q_OBJECT
public:
    explicit WasdfArduino(QObject *parent = 0);
private:
    QSerialPort *m_SerialPort;
signals:
    void analogPinReadingChangedDuringCalibration(int analogPinId, int newPosition);
    void fingerMoved(Finger finger, int newPosition);
public slots:
    void startInCalibrationMode();
    void start(const WasdfCalibrationConfiguration &calibrationConfig);
private slots:
    void handleSerialPortReadyRead();
};

#endif // WASDFARDUINO_H
