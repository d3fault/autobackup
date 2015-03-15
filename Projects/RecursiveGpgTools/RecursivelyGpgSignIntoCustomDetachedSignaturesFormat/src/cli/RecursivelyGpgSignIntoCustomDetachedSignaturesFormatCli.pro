#-------------------------------------------------
#
# Project created by QtCreator 2015-03-15T04:07:10
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../lib/recursivelygpgsignintocustomdetachedsignaturesformat.cpp \
    ../../../shared/recursivecustomdetachedgpgsignatures.cpp \
    recursivelygpgsignintocustomdetachedsignaturesformatcli.cpp

HEADERS += \
    ../lib/recursivelygpgsignintocustomdetachedsignaturesformat.h \
    ../../../shared/recursivecustomdetachedgpgsignatures.h \
    recursivelygpgsignintocustomdetachedsignaturesformatcli.h

INCLUDEPATH += ../lib/
INCLUDEPATH += ../../../shared/
