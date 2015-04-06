#-------------------------------------------------
#
# Project created by QtCreator 2015-04-06T00:03:12
#
#-------------------------------------------------

QT       += core serialport

QT       -= gui

TARGET = MusicFingersSynthesisToolkitCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    musicfingerssynthesistoolkit.cpp \
    ../../../MusicFingers/src/lib/finger.cpp \ #TODOoptional: pri
    musicfingerssynthesistoolkitcli.cpp

HEADERS += \
    musicfingerssynthesistoolkit.h \
    ../../../MusicFingers/src/lib/finger.h \
    musicfingerssynthesistoolkitcli.h

INCLUDEPATH += ../../../MusicFingers/src/
include(../../../MusicFingers/src/musicfingersserialportintegration.pri)
