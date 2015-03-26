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

SOURCES += cli/main.cpp \
    ../../shared/recursivecustomdetachedgpgsignatures.cpp \
    cli/recursivelygpgsignintocustomdetachedsignaturesformatcli.cpp

HEADERS += \
    ../../shared/recursivecustomdetachedgpgsignatures.h \
    cli/recursivelygpgsignintocustomdetachedsignaturesformatcli.h

INCLUDEPATH += ../../shared/

include(recursivelygpgsignintocustomdetachedsignaturesformat.pri)
include(../../../../GloballySharedClasses/savefileorstdout.pri)
