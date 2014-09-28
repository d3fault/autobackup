#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T12:35:44
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = SingleSignalMultipleSlots
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    singlesignalmultipleslotscli.cpp \
    foosignalclass.cpp \
    barslotclass.cpp \
    zedslotclass.cpp

HEADERS += \
    singlesignalmultipleslotscli.h \
    foosignalclass.h \
    barslotclass.h \
    zedslotclass.h
