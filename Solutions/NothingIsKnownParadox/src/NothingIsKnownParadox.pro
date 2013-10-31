#-------------------------------------------------
#
# Project created by QtCreator 2013-10-30T16:52:40
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = NothingIsKnownParadox
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    nothingisknownparadox.cpp \
    brain.cpp

include(../../../GloballySharedClasses/objectonthreadhelper.pri)

HEADERS += \
    nothingisknownparadox.h \
    brain.h
