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
    /home/user/text/Projects/LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.cpp

HEADERS += \
    quickdirtystaticgitweb.h \
    quickdirtystaticgitwebcli.h \
    /home/user/text/Projects/LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.h

INCLUDEPATH += /home/user/text/Projects/LastModifiedTimestampsSorter/src/lib/
include(/home/user/text/GloballySharedClasses/lastmodifiedtimestamp.pri)

LIBS += -lgit2
