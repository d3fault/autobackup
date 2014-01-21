#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T10:31:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AdDoublesRawMathLaunchSim
TEMPLATE = app


SOURCES += main.cpp\
        addoublesrawmathlaunchsimwidget.cpp \
    addoublesrawmathlaunchsim.cpp \
    addoublesrawmathlaunchsimgui.cpp

HEADERS  += addoublesrawmathlaunchsimwidget.h \
    addoublesrawmathlaunchsim.h \
    slotfillerpod.h \
    addoublesrawmathlaunchsimgui.h

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
