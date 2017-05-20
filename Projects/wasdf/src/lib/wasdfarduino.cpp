#include "wasdfarduino.h"

#include "wasdf.h"

WasdfArduino::WasdfArduino(QObject *parent)
    : QSerialPort(parent)
{ }
