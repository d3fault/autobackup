#-------------------------------------------------
#
# Project created by QtCreator 2014-07-05T00:01:15
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SimpleAsyncLibrarySlotInvokeToBackendThreadInvokeAndEventualSignalResponseCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    foo.cpp \
    bar.cpp

HEADERS += \
    foo.h \
    bar.h

include(../../../../../../GloballySharedClasses/objectonthreadgroup.pri)
