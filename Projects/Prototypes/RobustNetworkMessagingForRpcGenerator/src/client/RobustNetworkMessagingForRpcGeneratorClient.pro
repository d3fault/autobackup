QT       += core gui network

TARGET = RobustNetworkMessagingForRpcGeneratorClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    robustnetworkmessagingforrpcgeneratortestclient.cpp \
    testclient.cpp

HEADERS  += mainwidget.h \
    robustnetworkmessagingforrpcgeneratortestclient.h \
    testclient.h

include(../../../../GloballySharedClasses/debuggableStartableStoppableBackendAndFrontend.pri)
include(../../../../GloballySharedClasses/ssltcpclient.pri)
include(../../../../GloballySharedClasses/sslclienttestcerts.pri)
