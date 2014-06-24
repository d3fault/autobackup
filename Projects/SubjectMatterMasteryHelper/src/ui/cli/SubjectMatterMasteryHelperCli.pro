#-------------------------------------------------
#
# Project created by QtCreator 2014-06-23T20:13:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SubjectMatterMasteryHelperCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../subjectmattermasteryhelper.cpp \
    subjectmattermasteryhelpercli.cpp

HEADERS += \
    ../../subjectmattermasteryhelper.h \
    subjectmattermasteryhelpercli.h

include(../../../../../GloballySharedClasses/standardinputnotifier.pri)
