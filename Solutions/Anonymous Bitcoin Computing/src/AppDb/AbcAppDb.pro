#-------------------------------------------------
#
# Project created by QtCreator 2012-04-21T01:41:34
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = AbcAppDb
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    abcappdb.cpp \
    bankserverhelper.cpp \
    ourserverforwtfrontends.cpp \
    abcapplogic.cpp \
    Messages/bankserveractionrequest.cpp \
    Messages/applogicrequestresponse.cpp \
    Messages/applogicrequest.cpp \
    Messages/bankserveractionrequestresponse.cpp \
    ../shared/ssltcpserver.cpp

HEADERS += \
    abcappdb.h \
    bankserverhelper.h \
    ourserverforwtfrontends.h \
    abcapplogic.h \
    Messages/bankserveractionrequest.h \
    Messages/applogicrequestresponse.h \
    Messages/applogicrequest.h \
    Messages/bankserveractionrequestresponse.h \
    ../shared/ssltcpserver.h \
    ../shared/WtFrontEndAndAppDbProtocol.h




















