#-------------------------------------------------
#
# Project created by QtCreator 2013-05-02T15:50:34
#
#-------------------------------------------------

QT       += core gui

TARGET = RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    recursivelycopytoemptydestinationandeasytreehashalongtheway.cpp \
    recursivelycopytoemptydestinationandeasytreehashalongthewaybusiness.cpp

HEADERS  += mainwidget.h \
    recursivelycopytoemptydestinationandeasytreehashalongtheway.h \
    recursivelycopytoemptydestinationandeasytreehashalongthewaybusiness.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/easytreehasher.pri)
include(../../../GloballySharedClasses/easytreehashitem.pri)
