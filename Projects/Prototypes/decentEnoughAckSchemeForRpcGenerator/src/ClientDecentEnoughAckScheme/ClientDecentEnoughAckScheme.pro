#-------------------------------------------------
#
# Project created by QtCreator 2012-11-02T00:44:04
#
#-------------------------------------------------

QT       += core gui

TARGET = ClientDecentEnoughAckScheme
TEMPLATE = app


SOURCES += main.cpp\
        clientmainwidget.cpp \
    clientdecentenoughackschemetest.cpp \
    clientdecentenoughackschemebackend.cpp

HEADERS  += clientmainwidget.h \
    clientdecentenoughackschemetest.h \
    clientdecentenoughackschemebackend.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../../GloballySharedClasses/ssltcpclient.pri)
include(../../../../../GloballySharedClasses/sslclienttestcerts.pri)
