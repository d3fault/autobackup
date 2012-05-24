#-------------------------------------------------
#
# Project created by QtCreator 2012-05-05T05:19:22
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = BankServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    bankserver.cpp \
    bank.cpp \
    clientshelper.cpp \
    ../shared/ssltcpserver.cpp \
    autoGendRpcClientHypothetical/rpcclientshelper.cpp \
    autoGendRpcClientHypothetical/serverandprotocolknower.cpp

HEADERS += \
    bankserver.h \
    bank.h \
    clientshelper.h \
    ../shared/ssltcpserver.h \
    autoGendRpcClientHypothetical/rpcclientshelper.h \
    autoGendRpcClientHypothetical/serverandprotocolknower.h \
    ../RpcBankServer/ibank-rpc-output.h \
    ../RpcBankServer/RpcBankServerNetworkProtocol.h
