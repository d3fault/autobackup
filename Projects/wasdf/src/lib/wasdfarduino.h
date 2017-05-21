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
#if 0 //different readyRead handlers is better/cleaner :)
    enum class WasdfArduinoMode
    {
          Calibrating = 0x001
        , Report10fingerMovements = 0x010
        , ControlMouseAndKeyboard = 0x100 //TODOreq: I forget how to properly set up 'flags' that can be combined, this might be wrong (also calibrating can never be used with any other modes, but eh)
    };
#endif
    void openSerialPortIfNotOpen();

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
