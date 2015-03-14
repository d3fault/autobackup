#-------------------------------------------------
#
# Project created by QtCreator 2014-04-02T15:14:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseOrMotionOrMySexyFaceViewMaker
TEMPLATE = app

#DEFINES += MOUSE_OR_MOTION_OR_MY_SEXY_FACE_VIEW_MAKER_USE_BACKEND_THREAD

SOURCES += main.cpp\
    mouseormotionormysexyfaceviewmakergui.cpp \
    mouseormotionormysexyfaceviewmaker.cpp \
    mouseormotionormysexyfaceviewmakerwidget.cpp

HEADERS  += \
    mouseormotionormysexyfaceviewmakergui.h \
    mouseormotionormysexyfaceviewmaker.h \
    mouseormotionormysexyfaceviewmakerwidget.h

contains(DEFINES, MOUSE_OR_MOTION_OR_MY_SEXY_FACE_VIEW_MAKER_USE_BACKEND_THREAD)
{
    include(../../../GloballySharedClasses/objectonthreadhelper.pri)
}

RESOURCES += \
    mouseCursor.qrc
