#-------------------------------------------------
#
# Project created by QtCreator 2017-10-22T13:33:11
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = RequestResponseContract
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app

HEADERS += businessobject.h \
    businessobject2.h

SOURCES += main.cpp \
    businessobject.cpp \
    businessobject2.cpp    

include(AutoGeneratedRequestResponseContractGlue/BusinessObject/businessobjectrequestresponsecontracts.pri)