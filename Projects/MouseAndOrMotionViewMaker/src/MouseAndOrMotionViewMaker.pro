#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T17:34:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseAndOrMotionViewMaker
TEMPLATE = app


SOURCES += main.cpp\
        mouseandormotionviewmakerwidget.cpp \
    mouseandormotionviewmakergui.cpp \
    mouseandormotionviewmaker.cpp

HEADERS  += mouseandormotionviewmakerwidget.h \
    mouseandormotionviewmakergui.h \
    mouseandormotionviewmaker.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)

RESOURCES += \
    mouseCursor.qrc
