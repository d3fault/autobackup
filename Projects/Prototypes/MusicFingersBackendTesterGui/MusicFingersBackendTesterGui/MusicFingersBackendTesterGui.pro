#-------------------------------------------------
#
# Project created by QtCreator 2015-02-23T00:05:38
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicFingersBackendTesterGui
TEMPLATE = app

SOURCES += main.cpp\
        musicfingersbackendtesterwidget.cpp \
    musicfingersbackendtestergui.cpp

HEADERS  += musicfingersbackendtesterwidget.h \
    musicfingersbackendtestergui.h

include(../../../../GloballySharedClasses/objectonthreadgroup.pri)
include(../../../MusicFingers/src/musicfingers.pri)
