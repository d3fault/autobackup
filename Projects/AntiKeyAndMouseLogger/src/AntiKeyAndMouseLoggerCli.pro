#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T14:13:45
#
#-------------------------------------------------

QT       += core
QT       += gui #just for QKeySequence TODOreq

TARGET = AntiKeyAndMouseLoggerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ui/cli/antikeyandmouseloggercli.h

SOURCES += ui/cli/main.cpp \
    ui/cli/antikeyandmouseloggercli.cpp

include(antikeyandmouselogger.pri)
include(../../../GloballySharedClasses/standardinputnotifier.pri)

LIBS += -lcrypto++
