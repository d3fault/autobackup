QT       += core gui network

TARGET = MultiServerHelloer
TEMPLATE = app


SOURCES += main.cpp\
        serverwidget.cpp \
    multiserverhelloertest.cpp \
    multiserverhelloer.cpp \
    multiserverbusiness.cpp \
    ../ServerClientShared/networkmagic.cpp

HEADERS  += serverwidget.h \
    ../ServerClientShared/helloprotocol.h \
    multiserverhelloertest.h \
    multiserverhelloer.h \
    multiserverbusiness.h \
    ../ServerClientShared/networkmagic.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../../GloballySharedClasses/ssltcpserver.pri)
include(../../../../../GloballySharedClasses/ssltcpservertestcerts.pri)
