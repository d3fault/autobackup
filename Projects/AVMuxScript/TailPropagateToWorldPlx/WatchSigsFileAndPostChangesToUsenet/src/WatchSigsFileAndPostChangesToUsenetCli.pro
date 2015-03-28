#-------------------------------------------------
#
# Project created by QtCreator 2015-03-27T13:31:53
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = WatchSigsFileAndPostChangesToUsenetCli
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

HEADERS += cli/watchsigsfileandpostchangestousenetcli.h

SOURCES += cli/main.cpp \
    cli/watchsigsfileandpostchangestousenetcli.cpp

include(watchsigsfileandpostchangestousenet.pri)
include(../../../../RecursiveGpgTools/shared/recursivecustomdetachedgpgsignatures.pri)
include(../../../../../GloballySharedClasses/standardinputnotifier.pri)
