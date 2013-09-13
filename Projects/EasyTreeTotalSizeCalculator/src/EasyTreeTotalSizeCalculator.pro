#-------------------------------------------------
#
# Project created by QtCreator 2013-09-13T11:20:31
#
#-------------------------------------------------

QT       += core gui

TARGET = EasyTreeTotalSizeCalculator
TEMPLATE = app


SOURCES += main.cpp\
        easytreetotalsizecalculatorwidget.cpp \
    easytreetotalsizecalculatorgui.cpp \
    easytreetotalsizecalculator.cpp

HEADERS  += easytreetotalsizecalculatorwidget.h \
    easytreetotalsizecalculatorgui.h \
    easytreetotalsizecalculator.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/easytreeparser.pri)
