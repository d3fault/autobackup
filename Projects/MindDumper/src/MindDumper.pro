#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T13:56:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MindDumper
TEMPLATE = app

SOURCES += main.cpp\
        minddumpermainwindow.cpp \
    minddumpdocument.cpp

HEADERS  += minddumpermainwindow.h \
    minddumpdocument.h

include(../../../GloballySharedClasses/qtsystemsignalhandler.pri)
