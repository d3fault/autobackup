#-------------------------------------------------
#
# Project created by QtCreator 2016-01-28T22:07:45
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = QuickDirtyStaticGitWebCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    quickdirtystaticgitweb.cpp \
    quickdirtystaticgitwebcli.cpp \
    ../../LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.cpp

HEADERS += \
    quickdirtystaticgitweb.h \
    quickdirtystaticgitwebcli.h \
    ../../LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.h

INCLUDEPATH += ../../LastModifiedTimestampsSorter/src/lib/
include(../../../GloballySharedClasses/lastmodifiedtimestamp.pri)

LIBS += -lgit2
