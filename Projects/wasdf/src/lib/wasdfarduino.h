#ifndef WASDFARDUINO_H
#define WASDFARDUINO_H

#include <QtSerialPort/QSerialPort>

class WasdfArduino : private QSerialPort
{
    Q_OBJECT
public:
    explicit WasdfArduino(QObject *parent = 0);
signals:
    void fingerMoved(Finger finger, int newPosition);
};

#endif // WASDFARDUINO_H
