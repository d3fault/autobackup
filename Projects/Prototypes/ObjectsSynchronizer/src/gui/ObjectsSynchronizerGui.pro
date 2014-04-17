#-------------------------------------------------
#
# Project created by QtCreator 2014-04-15T02:25:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ObjectsSynchronizerGui
TEMPLATE = app


SOURCES += main.cpp\
        objectssynchronizerwidget.cpp \
    ../lib/objectssynchronizer.cpp

HEADERS  += objectssynchronizerwidget.h \
    ../lib/objectssynchronizer.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
