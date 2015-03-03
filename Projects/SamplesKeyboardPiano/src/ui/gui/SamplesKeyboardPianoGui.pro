#-------------------------------------------------
#
# Project created by QtCreator 2015-02-28T00:32:10
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SamplesKeyboardPianoGui
TEMPLATE = app


SOURCES += main.cpp\
    ../../lib/sampleskeyboardpiano.cpp \
    sampleskeyboardpianowidget.cpp \
    sampleskeyboardpianogui.cpp \
    ../../lib/sampleskeyboardpianoprofile.cpp \
    ../../lib/sampleskeyboardpianoprofileconfiguration.cpp \
    sampleskeyboardpianoprofilemanagerwidget.cpp

HEADERS  += \
    ../../lib/sampleskeyboardpiano.h \
    sampleskeyboardpianowidget.h \
    sampleskeyboardpianogui.h \
    ../../lib/sampleskeyboardpianoprofile.h \
    ../../lib/sampleskeyboardpianoprofileconfiguration.h \
    sampleskeyboardpianoprofilemanagerwidget.h

INCLUDEPATH += ../../lib
