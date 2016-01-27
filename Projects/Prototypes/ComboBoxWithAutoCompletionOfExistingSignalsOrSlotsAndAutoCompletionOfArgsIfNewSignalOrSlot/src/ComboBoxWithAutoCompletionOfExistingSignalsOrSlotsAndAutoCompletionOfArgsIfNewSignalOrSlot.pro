#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T18:31:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot
TEMPLATE = app


SOURCES += main.cpp

include(../../../../GloballySharedClasses/comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.pri)

DEFINES += NDEBUG
DEFINES += _GNU_SOURCE
DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += $$system('llvm-config --includedir')
LIBS += -L$$system('llvm-config --libdir') -lclang
