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
    minddumpdocument.cpp \
    minddumperfirstlaunchdirselector.cpp

HEADERS  += minddumpermainwindow.h \
    minddumpdocument.h \
    minddumperfirstlaunchdirselector.h

include(../../../GloballySharedClasses/qtsystemsignalhandler.pri)
include(../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
