#-------------------------------------------------
#
# Project created by QtCreator 2014-04-10T14:23:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui
TEMPLATE = app


SOURCES += main.cpp\
        runtimedynamiciterationsdeterminerretainingresponsivenesswidget.cpp \
    runtimedynamiciterationsdeterminerretainingresponsivenessgui.cpp \
    ../lib/runtimedynamiciterationsdeterminerretainingresponsiveness.cpp

HEADERS  += runtimedynamiciterationsdeterminerretainingresponsivenesswidget.h \
    runtimedynamiciterationsdeterminerretainingresponsivenessgui.h \
    ../lib/runtimedynamiciterationsdeterminerretainingresponsiveness.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
