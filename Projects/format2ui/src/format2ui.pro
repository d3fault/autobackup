#-------------------------------------------------
#
# Project created by QtCreator 2018-02-02T11:24:39
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = format2ui
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += format2ui.h \
    iuigenerator.h \
    qtwidgetsuigenerator.h \
    uivariable.h \
    uigeneratorfactory.h \
    qtcliuigenerator.h \
    uigeneratorformat.h

SOURCES += main.cpp \
    format2ui.cpp \
    qtwidgetsuigenerator.cpp \
    uigeneratorfactory.cpp \
    qtcliuigenerator.cpp \
    iuigenerator.cpp
