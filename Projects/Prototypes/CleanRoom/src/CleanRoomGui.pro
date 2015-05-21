#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T12:50:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CleanRoomGui
TEMPLATE = app

HEADERS  += gui/desktop/cleanroomwidget.h \
    gui/desktop/cleanroomgui.h \
    gui/desktop/cleanroomguisessionresponder.h

SOURCES += gui/desktop/main.cpp\
        gui/desktop/cleanroomwidget.cpp \
    gui/desktop/cleanroomgui.cpp \
    gui/desktop/cleanroomguisessionresponder.cpp

include(cleanroom.pri)
