#-------------------------------------------------
#
# Project created by QtCreator 2014-04-17T02:41:10
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LastModifiedTimestampsSorterCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../lib/lastmodifiedtimestampssorter.cpp \
    lastmodifiedtimestampssortercli.cpp

HEADERS += \
    ../lib/lastmodifiedtimestampssorter.h \
    lastmodifiedtimestampssortercli.h

include(../../../../GloballySharedClasses/simplifiedlastmodifiedtimestamp.pri)
