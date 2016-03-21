#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T00:40:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = FunctionDecomposerWithNewTypeDetectorGui
TEMPLATE = app
CONFIG += precompile_header

PRECOMPILED_HEADER = libclangpch.h #not only do i want faster compiles, but i want to not see the thousands of llvm/clang warnings... so i can see the ones relevant to my app. appears to work if the file using the precompiled header doesn't change <3

SOURCES += main.cpp\
        functiondecomposerwithnewtypedetectorwidget.cpp \
    functiondecomposerwithnewtypedetector.cpp \
    functiondecomposerwithnewtypedetectorgui.cpp

HEADERS  += functiondecomposerwithnewtypedetectorwidget.h \
    functiondecomposerwithnewtypedetector.h \
    functiondecomposerwithnewtypedetectorgui.h

DEFINES += NDEBUG
DEFINES += _GNU_SOURCE
DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += $$system('llvm-config --includedir')
LIBS += -L$$system('llvm-config --libdir') -lclangTooling -lclangDriver -lclangFrontend -lclangParse -lclangSema -lclangEdit -lclangAnalysis -lclangLex -lclangSerialization -lLLVMBitReader -lclangAST -lclangBasic -lLLVMMCParser -lLLVMMC -lLLVMOption -lLLVMSupport -lz -lpthread -lffi -ltinfo -ldl -lm -lclang
