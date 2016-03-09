#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T14:13:45
#
#-------------------------------------------------

QT       += core

TARGET = AntiKeyAndMouseLoggerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ui/cli/antikeyandmouseloggercli.h

SOURCES += ui/cli/main.cpp \
    ui/cli/antikeyandmouseloggercli.cpp

include(antikeyandmouselogger.pri)
include(../../../GloballySharedClasses/standardinputnotifier.pri)
include(../../../GloballySharedClasses/qtsystemsignalhandler.pri)

LIBS += -lcrypto++
