#-------------------------------------------------
#
# Project created by QtCreator 2017-09-19T18:04:04
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = UserInterfaceSkeletonGeneratorCli
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    userinterfaceskeletongeneratorcli.h \
    userinterfaceskeletongenerator.h \
    iuserinterfaceimplstubgenerator.h \
    cliimplstubgenerator.h \
    userinterfaceskeletongeneratordata.h

SOURCES += main.cpp \
    userinterfaceskeletongeneratorcli.cpp \
    userinterfaceskeletongenerator.cpp \
    cliimplstubgenerator.cpp \
    userinterfaceskeletongeneratordata.cpp
