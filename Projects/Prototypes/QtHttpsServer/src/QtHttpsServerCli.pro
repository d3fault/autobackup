#-------------------------------------------------
#
# Project created by QtCreator 2014-12-29T16:50:54
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = QtHttpsServerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    qthttpsserver.cpp \
    qthttpsclient.cpp \
    qthttpsservercli.cpp

HEADERS += \
    qthttpsserver.h \
    qthttpsclient.h \
    qthttpsservercli.h

include(../../../../GloballySharedClasses/standardinputnotifier.pri)
