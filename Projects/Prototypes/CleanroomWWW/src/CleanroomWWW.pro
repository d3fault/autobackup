#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T01:06:19
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = CleanRoomWWW

CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

HEADERS += gui/web/cleanroomwww.h \
    gui/web/cleanroomwtsessionprovider.h

SOURCES += gui/web/main.cpp \
    gui/web/cleanroomwww.cpp \
    gui/web/cleanroomwtsessionprovider.cpp

include(cleanroom.pri)
