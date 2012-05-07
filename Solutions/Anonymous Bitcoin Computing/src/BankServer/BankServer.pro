#-------------------------------------------------
#
# Project created by QtCreator 2012-05-05T05:19:22
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = BankServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    bankserver.cpp \
    bank.cpp \
    clientshelper.cpp \
    ../shared/ssltcpserver.cpp

HEADERS += \
    bankserver.h \
    bank.h \
    clientshelper.h \
    ../shared/ssltcpserver.h










