#-------------------------------------------------
#
# Project created by QtCreator 2014-12-29T16:50:54
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = QtHttpServerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    qthttpserver.cpp \
    qthttpclient.cpp \
    qthttpservercli.cpp

HEADERS += \
    qthttpserver.h \
    qthttpclient.h \
    qthttpservercli.h

include(../../../../GloballySharedClasses/standardinputnotifier.pri)
