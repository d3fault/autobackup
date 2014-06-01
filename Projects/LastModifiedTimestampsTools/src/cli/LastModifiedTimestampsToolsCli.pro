#-------------------------------------------------
#
# Project created by QtCreator 2014-06-01T06:03:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LastModifiedTimestampsToolsCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lastmodifiedtimestampstoolscli.cpp \
    ../lib/lastmodifiedtimestampstools.cpp

HEADERS += \
    lastmodifiedtimestampstoolscli.h \
    ../lib/lastmodifiedtimestampstools.h

include(../../../../GloballySharedClasses/lastmodifiedtimestamp.pri)
include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
