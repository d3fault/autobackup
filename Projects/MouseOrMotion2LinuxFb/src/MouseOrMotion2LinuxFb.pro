#-------------------------------------------------
#
# Project created by QtCreator 2016-02-18T17:41:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseOrMotion2LinuxFb
TEMPLATE = app

HEADERS  += mouseormotion2linuxfb.h

SOURCES += main.cpp \
    mouseormotion2linuxfb.cpp

include(../../../GloballySharedClasses/mouseormotionorperiodofinactivitydetector.pri)
include(../../../GloballySharedClasses/mouseormotionrectgrabberwithperiodofinactivitydetection.pri)
