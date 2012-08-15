#-------------------------------------------------
#
# Project created by QtCreator 2012-08-15T13:30:25
#
#-------------------------------------------------

QT       += core gui

TARGET = RobustNetworkMessagingForRpcGenerator
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    robustnetworkmessagingforrpcgeneratortest.cpp \
    testserver.cpp \
    testclient.cpp

HEADERS  += mainwidget.h \
    robustnetworkmessagingforrpcgeneratortest.h \
    testserver.h \
    testclient.h

include(../../../../GloballySharedClasses/debuggableStartableStoppableBackendAndFrontend.pri)
include(../../../../GloballySharedClasses/ssltcpserver.pri)
include(../../../../GloballySharedClasses/ssltcpservertestcerts.pri)
include(../../../../GloballySharedClasses/ssltcpclient.pri)
include(../../../../GloballySharedClasses/sslclienttestcerts.pri)
