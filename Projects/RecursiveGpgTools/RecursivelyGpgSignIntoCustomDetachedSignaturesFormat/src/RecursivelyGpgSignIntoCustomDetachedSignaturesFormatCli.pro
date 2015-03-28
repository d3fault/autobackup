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
    cli/recursivelygpgsignintocustomdetachedsignaturesformatcli.cpp

HEADERS += \
    cli/recursivelygpgsignintocustomdetachedsignaturesformatcli.h

include(recursivelygpgsignintocustomdetachedsignaturesformat.pri)
include(../../shared/recursivecustomdetachedgpgsignatures.pri)
include(../../../../GloballySharedClasses/savefileorstdout.pri)
