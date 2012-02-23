#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T13:29:46
#
#-------------------------------------------------

QT       += core gui network

TARGET = SecureClient
TEMPLATE = app


SOURCES += main.cpp\
        mainclientwidget.cpp \
    secureclient.cpp

HEADERS  += mainclientwidget.h \
    secureclient.h \
    ../shared/protocol.h


RESOURCES += \
    MyCA.qrc