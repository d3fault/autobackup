#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T20:21:14
#
#-------------------------------------------------

QT       += core serialport
QT       -= gui

TARGET = wasdf-cli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += cli/main.cpp

include(wasdf.pri)
