#-------------------------------------------------
#
# Project created by QtCreator 2013-05-01T01:11:21
#
#-------------------------------------------------

QT       += core gui

TARGET = EasyTreeGui
TEMPLATE = app


SOURCES += main.cpp\
        easytreeguiwidget.cpp \
    easytreegui.cpp \
    easytreeguibusiness.cpp

HEADERS  += easytreeguiwidget.h \
    easytreegui.h \
    easytreeguibusiness.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/easytree.pri)
