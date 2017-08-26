#ifndef WASDFARDUINO_H
#define WASDFARDUINO_H

#include <QObject>

#include <QTextStream>

#include "wasdf.h"

class QSerialPort;

class QtIoDeviceChecksummedMessageReader;

class WasdfArduino : public QObject
{
    Q_OBJECT
public:
    explicit WasdfArduino(QObject *parent = 0);
private:
    void openSerialPortIfNotOpen();
    static QString jsonObjectToQString(const QJsonObject &jsonObject);
    void sendRawCommandToArduino(const QString &command);
    bool detectAndHandleDebugMessage(const QByteArray &messageJson);

    WasdfCalibrationConfiguration m_CalibrationConfig;
    QSerialPort *m_SerialPort;
    QtIoDeviceChecksummedMessageReader *m_ChecksummedMessageReader;
    QTextStream m_SerialPortTextStream;
signals:
    void e(const QString &msg);
    void analogPinReadingChangedDuringCalibration(int analogPinId, int newPosition);
    void fingerMoved(Finger finger, int newPosition);
public slots:
    void startInCalibrationMode();
    void start(const WasdfCalibrationConfiguration &calibrationConfig);
private slots:
    void handleCalibrationModeMessageReceived(const QByteArray &messageJson);
    void handleRegularModeMessageReceived(const QByteArray &messageJson);
    void dumpRawSerialStreamByPeeking();
};

#endif // WASDFARDUINO_H
