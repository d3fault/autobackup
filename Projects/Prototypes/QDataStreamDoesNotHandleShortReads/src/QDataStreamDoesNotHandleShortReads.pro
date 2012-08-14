#-------------------------------------------------
#
# Project created by QtCreator 2012-08-13T15:12:40
#
#-------------------------------------------------

QT       += core gui network

TARGET = QDataStreamDoesNotHandleShortReads
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    datastreamdoesnothandleshortreadsbackend.cpp \
    testclient.cpp \
    testserver.cpp

HEADERS  += mainwidget.h \
    datastreamdoesnothandleshortreadsbackend.h \
    testclient.h \
    testserver.h \
    simpleassprotocol.h

include(../../../../GloballySharedClasses/debuggableStartableStoppableBackendAndFrontend.pri)
include(../../../../GloballySharedClasses/ssltcpserver.pri)
include(../../../../GloballySharedClasses/ssltcpservertestcerts.pri)
include(../../../../GloballySharedClasses/ssltcpclient.pri)
include(../../../../GloballySharedClasses/sslclienttestcerts.pri)
