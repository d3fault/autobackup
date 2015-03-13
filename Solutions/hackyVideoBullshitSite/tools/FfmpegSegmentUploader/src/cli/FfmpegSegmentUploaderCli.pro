#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T23:09:11
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = FfmpegSegmentUploaderCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ffmpegsegmentuploadercli.cpp \
    ../lib/ffmpegsegmentuploader.cpp

HEADERS += \
    ffmpegsegmentuploadercli.h \
    ../lib/ffmpegsegmentuploader.h

include(../../../../../../Projects/SftpUploaderAndRenamerQueue/src/sftpuploadandrenamerqueue.pri)
include(../../../../../../GloballySharedClasses/standardinputnotifier.pri)
