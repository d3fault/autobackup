#-------------------------------------------------
#
# Project created by QtCreator 2012-03-01T03:36:40
#
#-------------------------------------------------

QT       += core gui network

TARGET = BitcoinCommunicatingServer
TEMPLATE = app


SOURCES += main.cpp\
        debugserverview.cpp \
    ssltcpserver.cpp \
    bank.cpp \
    appclienthelper.cpp \
    bankdb.cpp

HEADERS  += debugserverview.h \
    ssltcpserver.h \
    ../sharedProtocol/sharedprotocol.h \
    bank.h \
    appclienthelper.h \
    bankdb.h

RESOURCES += \
    SslFiles.qrc