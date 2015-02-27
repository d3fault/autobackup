#-------------------------------------------------
#
# Project created by QtCreator 2015-02-26T20:44:05
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = VerifyCustomDetachedGpgSignaturesCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../lib/verifycustomdetachedgpgsignatures.cpp \
    verifycustomdetachedgpgsignaturescli.cpp

HEADERS += \
    ../lib/verifycustomdetachedgpgsignatures.h \
    verifycustomdetachedgpgsignaturescli.h

INCLUDEPATH += ../lib/
