#-------------------------------------------------
#
# Project created by QtCreator 2014-07-05T01:09:35
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = UnitOfExecutionDelayerWhenAnyChildDependencyIsOnDifferentThreadCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += zop.cpp \
    foo.cpp \
    bar.cpp \
    main.cpp

HEADERS += \
    zop.h \
    foo.h \
    bar.h

include(../../../../../../GloballySharedClasses/objectonthreadgroup.pri)
