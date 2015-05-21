#-------------------------------------------------
#
# Project created by QtCreator 2015-05-21T12:51:13
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = RpcGenerator3Cli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    rpcgenerator.cpp \
    rpcgeneratorcli.cpp

HEADERS += rpcgenerator.h \
    rpcgeneratorcli.h \
    api.h

RESOURCES += \
    rpcbusinesstemplatestousewhengenerating.qrc
