#-------------------------------------------------
#
# Project created by QtCreator 2014-03-20T21:37:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HotteeCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    hotteecli.cpp \
    ../lib/hottee.cpp

HEADERS += \
    hotteecli.h \
    ../lib/hottee.h

LIBS += -lboost_filesystem

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
