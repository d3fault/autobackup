#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T01:49:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GitUnrollRerollCensorshipMachineCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    gitunrollrerollcensorshipmachinecli.cpp

HEADERS += \
    gitunrollrerollcensorshipmachinecli.h

include(../lib/gitunrollrerollcensorshipmachine.pri)
include(../../../../GloballySharedClasses/easytreeparser.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)
include(../../../../GloballySharedClasses/githelper.pri)
