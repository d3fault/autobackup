#-------------------------------------------------
#
# Project created by QtCreator 2017-09-23T17:31:34
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = StupidKeyValueContentTrackerCli
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += stupidkeyvaluecontenttracker.h \
    ikeyvaluestoremutation.h \
    keyvaluestoremutation_add.h \
    stupidkeyvaluecontenttrackercli.h

SOURCES += main.cpp \
    stupidkeyvaluecontenttracker.cpp \
    keyvaluestoremutation_add.cpp \
    stupidkeyvaluecontenttrackercli.cpp

include(../../TimeAndData_Timeline/src/timeanddata_timeline.pri)
