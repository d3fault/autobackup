#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T19:28:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AntiKeyAndMouseLoggerGui
TEMPLATE = app

HEADERS  += ui/gui/antikeyandmouseloggerwidget.h

SOURCES += ui/gui/main.cpp\
        ui/gui/antikeyandmouseloggerwidget.cpp

include(antikeyandmouselogger.pri)
