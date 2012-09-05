QT       += core gui network

TARGET = RobustNetworkMessagingForRpcGeneratorServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    robustnetworkmessagingforrpcgeneratortestserver.cpp \
    testserver.cpp

HEADERS  += mainwidget.h \
    robustnetworkmessagingforrpcgeneratortestserver.h \
    testserver.h

include(../../../../../GloballySharedClasses/debuggableStartableStoppableBackendAndFrontend.pri)
include(../../../../../GloballySharedClasses/ssltcpserver.pri)
include(../../../../../GloballySharedClasses/ssltcpservertestcerts.pri)
