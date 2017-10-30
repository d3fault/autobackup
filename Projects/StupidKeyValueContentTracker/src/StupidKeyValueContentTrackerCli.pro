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

HEADERS += stupidkeyvaluecontenttrackercli.h

SOURCES +=	main.cpp \
		stupidkeyvaluecontenttrackercli.cpp

include(stupidkeyvaluecontenttracker.pri)
include(../../../GloballySharedClasses/standardinputnotifier.pri)
