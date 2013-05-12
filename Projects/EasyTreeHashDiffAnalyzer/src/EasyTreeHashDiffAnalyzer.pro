#-------------------------------------------------
#
# Project created by QtCreator 2013-05-12T06:32:04
#
#-------------------------------------------------

QT       += core gui

TARGET = EasyTreeHashDiffAnalyzer
TEMPLATE = app


SOURCES += main.cpp\
        easytreehashdiffanalyzerwidget.cpp \
    easytreehashdiffanalyzer.cpp \
    easytreehashdiffanalyzergui.cpp \
    easytreehashdiffanalyzerbusiness.cpp

HEADERS  += easytreehashdiffanalyzerwidget.h \
    easytreehashdiffanalyzer.h \
    easytreehashdiffanalyzergui.h \
    easytreehashdiffanalyzerbusiness.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/easytreeparser.pri)