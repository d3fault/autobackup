#-------------------------------------------------
#
# Project created by QtCreator 2015-05-17T19:38:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IcePick
TEMPLATE = app

SOURCES += gui/main.cpp\
        gui/icepickwidget.cpp \
    lib/icepick.cpp \
    gui/icepickgui.cpp

HEADERS  += gui/icepickwidget.h \
    lib/icepick.h \
    gui/icepickgui.h

include(/home/user/text/GloballySharedClasses/objectonthreadgroup.pri)
