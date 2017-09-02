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
    void openSerialPort();
    static QString jsonObjectToQString(const QJsonObject &jsonObject);
    void sendRawCommandToArduino(const QString &command);
    bool detectAndHandleDebugMessage(const QByteArray &messageJson);

    QSerialPort *m_SerialPort;
    QtIoDeviceChecksummedMessageReader *m_ChecksummedMessageReader;
    QTextStream m_SerialPortTextStream;
signals:
    void e(const QString &msg);
    void analogPinReadingChanged(int analogPinId, int newPosition);
public slots:
    void start();
    void configure(const WasdfCalibrationConfiguration &calibrationConfig);
private slots:
    void handleChecksummedMessageReceived(const QByteArray &messageJson);
    //void handleRegularModeMessageReceived(const QByteArray &messageJson);
    void dumpRawSerialStreamByPeeking();
};

#endif // WASDFARDUINO_H
