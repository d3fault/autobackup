#-------------------------------------------------
#
# Project created by QtCreator 2016-02-13T13:32:26
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseOrMotionOrPeriodOfInactivityDetectorGui
TEMPLATE = app

SOURCES += main.cpp \
    mouseormotionorperiodofinactivitydetectordebugoutput.cpp

include(../../../../GloballySharedClasses/mouseormotionorperiodofinactivitydetector.pri)

HEADERS += \
    mouseormotionorperiodofinactivitydetectordebugoutput.h
