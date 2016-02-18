#-------------------------------------------------
#
# Project created by QtCreator 2016-02-17T16:15:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui
TEMPLATE = app

HEADERS  += mouseormotionrectgrabberwithperiodofinactivitydetectionwidget.h

SOURCES += main.cpp\
        mouseormotionrectgrabberwithperiodofinactivitydetectionwidget.cpp

include(../../../../GloballySharedClasses/mouseormotionorperiodofinactivitydetector.pri)
include(../../../../GloballySharedClasses/mouseormotionrectgrabberwithperiodofinactivitydetection.pri)
