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
    bankdb.cpp \
    bitcoinhelper.cpp \
    bitcoinpoller.cpp

HEADERS  += debugserverview.h \
    ssltcpserver.h \
    ../sharedProtocol/sharedprotocol.h \
    ../sharedProtocol/userBankAccount.h \
    bank.h \
    appclienthelper.h \
    bankdb.h \
    bitcoinhelper.h \
    bitcoinpoller.h

RESOURCES += \
    SslFiles.qrc

