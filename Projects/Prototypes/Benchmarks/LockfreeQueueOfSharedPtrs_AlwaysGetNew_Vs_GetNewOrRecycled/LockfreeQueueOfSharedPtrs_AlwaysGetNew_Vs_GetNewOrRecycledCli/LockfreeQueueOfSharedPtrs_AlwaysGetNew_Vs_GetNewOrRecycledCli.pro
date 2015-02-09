#-------------------------------------------------
#
# Project created by QtCreator 2015-02-06T09:34:26
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = LockfreeQueueOfSharedPtrs_AlwaysGetNew_Vs_GetNewOrRecycledCli
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

HEADERS += ../../../../../GloballySharedClasses/threading/lockfreequeueofsharedptrs.h
SOURCES += main.cpp

INCLUDEPATH += ../../../../../GloballySharedClasses/threading/

LIBS += -lboost_system -lboost_thread
