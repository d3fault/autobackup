#-------------------------------------------------
#
# Project created by QtCreator 2014-04-02T15:14:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseOrMotionOrMySexyFaceViewMaker

TEMPLATE = app


SOURCES += main.cpp\
    mouseormotionormysexyfaceviewmakergui.cpp \
    mouseormotionormysexyfaceviewmaker.cpp \
    mouseormotionormysexyfaceviewmakerwidget.cpp

HEADERS  += \
    mouseormotionormysexyfaceviewmakergui.h \
    mouseormotionormysexyfaceviewmaker.h \
    mouseormotionormysexyfaceviewmakerwidget.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)

RESOURCES += \
    mouseCursor.qrc
