#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T11:44:37
#
#-------------------------------------------------

QT       += core gui network

TARGET = SecureServer
TEMPLATE = app


SOURCES += main.cpp\
        mainserverwidget.cpp \
    secureserver.cpp \
    ssltcpserver.cpp

HEADERS  += mainserverwidget.h \
    secureserver.h \
    ssltcpserver.h \
    ../shared/protocol.h




