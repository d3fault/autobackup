#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T18:31:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot
TEMPLATE = app

HEADERS += resultstatechangedlistener.h \
    ../../../../GloballySharedClasses/libclang/libclangpch.h

SOURCES += main.cpp \
    resultstatechangedlistener.cpp

include(../../../../GloballySharedClasses/libclangfunctiondeclarationparser.pri)
include(../../../../GloballySharedClasses/comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.pri)

DEFINES += NDEBUG
DEFINES += _GNU_SOURCE
DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += $$system('llvm-config --includedir')
LIBS += -L$$system('llvm-config --libdir') -lclangTooling -lclangDriver -lclangFrontend -lclangParse -lclangSema -lclangEdit -lclangAnalysis -lclangLex -lclangSerialization -lLLVMBitReader -lclangAST -lclangBasic -lLLVMMCParser -lLLVMMC -lLLVMOption -lLLVMSupport -lz -lpthread -lffi -ltinfo -ldl -lm -lclang

#CONFIG += precompile_header
#PRECOMPILED_HEADER = libclangpch.h
