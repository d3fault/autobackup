#-------------------------------------------------
#
# Project created by QtCreator 2011-12-27T12:38:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = consoleLibAvReadAviHeaderFromMemoryTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

LIBS += -lavcodec -lavformat -lavutil -lswscale -lz
