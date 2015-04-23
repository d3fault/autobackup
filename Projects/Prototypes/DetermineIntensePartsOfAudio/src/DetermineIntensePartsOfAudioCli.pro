#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T06:11:16
#
#-------------------------------------------------

QT       += core gui #y da fuq isn't QImage in the core lib?

TARGET = DetermineIntensePartsOfAudioCli
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

SOURCES += cli/main.cpp \
    cli/determineintensepartsofaudiocli.cpp

include(determineintensepartsofaudio.pri)

HEADERS += \
    cli/determineintensepartsofaudiocli.h
