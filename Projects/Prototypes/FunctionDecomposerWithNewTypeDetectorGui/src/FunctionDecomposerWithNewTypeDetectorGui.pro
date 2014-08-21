#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T00:40:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FunctionDecomposerWithNewTypeDetectorGui
TEMPLATE = app
CONFIG += precompile_header

PRECOMPILED_HEADER = libclangpch.h #not only do i want faster compiles, but i want to not see the thousands of llvm/clang warnings... so i can see the ones relevant to my app. damn, it doesn't work. but i think compile time did speed up a tiny bit

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
INCLUDEPATH += "/usr/lib/llvm-3.4/include"
LIBS += -L/usr/lib/llvm-3.4/lib -lpthread -lffi -ltinfo -ldl -lm -lclangTooling -lclangDriver -lclangFrontend -lclangParse -lclangSema -lclangEdit -lclangAnalysis -lclangLex -lclangSerialization -lclangAST -lclangBasic -lclang
