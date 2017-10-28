#-------------------------------------------------
#
# Project created by QtCreator 2017-09-18T14:40:47
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget
TEMPLATE = app

CONFIG += C++11

HEADERS  += libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h \
    stkfileloopiodevice.h \
    audiooutput.h \
    libstk_loopaudiofileandmodifyitspitchusingaslider.h

SOURCES += main.cpp \
        libstk_loopaudiofileandmodifyitspitchusingasliderwidget.cpp \
    stkfileloopiodevice.cpp \
    audiooutput.cpp \
    libstk_loopaudiofileandmodifyitspitchusingaslider.cpp

LIBS += -lstk

include(../../../../GloballySharedClasses/objectonthreadgroup.pri)
