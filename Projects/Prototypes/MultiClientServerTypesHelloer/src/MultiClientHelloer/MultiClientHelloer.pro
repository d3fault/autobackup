#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T11:03:50
#
#-------------------------------------------------

QT       += core gui network

TARGET = MultiClientHelloer
TEMPLATE = app


SOURCES += main.cpp\
        clientwidget.cpp \
    multiclienthelloertest.cpp \
    multiclientbusiness.cpp \
    multiclienthelloer.cpp

HEADERS  += clientwidget.h \
    multiclienthelloertest.h \
    multiclientbusiness.h \
    multiclienthelloer.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../../GloballySharedClasses/ssltcpclient.pri)
include(../../../../../GloballySharedClasses/sslclienttestcerts.pri)
