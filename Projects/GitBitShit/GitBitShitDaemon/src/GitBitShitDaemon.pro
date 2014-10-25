#-------------------------------------------------
#
# Project created by QtCreator 2014-10-24T18:21:32
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = GitBitShitDaemon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    gitbitshitdaemon.cpp \
    gitbitshitdaemonclient.cpp

HEADERS += \
    gitbitshitdaemon.h \
    ../../GitBitShitShared/gitbitshitshared.h \
    gitbitshitdaemonclient.h

INCLUDEPATH += ../../GitBitShitShared/
