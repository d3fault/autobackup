#-------------------------------------------------
#
# Project created by QtCreator 2017-10-30T10:32:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SocialNetworkTimeline
TEMPLATE = app
CONFIG += c++11

HEADERS  += socialnetworktimelinewidget.h \
    socialnetworktimeline.h

SOURCES += main.cpp \
    socialnetworktimelinewidget.cpp \
    socialnetworktimeline.cpp

include(../../StupidKeyValueContentTracker/src/stupidkeyvaluecontenttracker.pri)
