#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T20:21:14
#
#-------------------------------------------------

QT       += core serialport
QT       -= gui

TARGET = wasdf-cli
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += WASDF_PC

HEADERS += cli/wasdfcli.h

INCLUDEPATH *= ../assets/ArduinoSketches/ArduinoWriteChangesOnAnalogPinsToSerial/

SOURCES += cli/main.cpp \
    cli/wasdfcli.cpp \
    ../assets/ArduinoSketches/ArduinoWriteChangesOnAnalogPinsToSerial/ArduinoWriteChangesOnAnalogPinsToSerial.ino

include(../../../GloballySharedClasses/qtiodevicechecksummedmessagereader.pri)
include(wasdf.pri)
