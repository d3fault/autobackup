#-------------------------------------------------
#
# Project created by QtCreator 2015-02-26T20:44:05
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = RecursivelyVerifyCustomDetachedGpgSignaturesCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    recursivelyverifycustomdetachedgpgsignaturescli.cpp \
    ../lib/recursivelyverifycustomdetachedgpgsignatures.cpp \
    ../../../shared/recursivecustomdetachedgpgsignatures.cpp

HEADERS += \
    recursivelyverifycustomdetachedgpgsignaturescli.h \
    ../lib/recursivelyverifycustomdetachedgpgsignatures.h \
    ../../../shared/recursivecustomdetachedgpgsignatures.h

INCLUDEPATH += ../lib/
INCLUDEPATH += ../../../shared/
