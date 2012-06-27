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
    bankdbhelper.cpp \
    clientshelper.cpp \
    ../shared/ssltcpserver.cpp \
    autoGendRpcClientHypothetical/rpcclientshelper.cpp \
    autoGendRpcClientHypothetical/serverandprotocolknower.cpp \
    couchbaseclusterdbhelper.cpp

HEADERS += \
    bankserver.h \
    bank.h \
    bankdbhelper.h \
    clientshelper.h \
    ../shared/ssltcpserver.h \
    autoGendRpcClientHypothetical/rpcclientshelper.h \
    autoGendRpcClientHypothetical/serverandprotocolknower.h \
    ../RpcBankServer/rpcbankservernetworkmessage.h \
    ../RpcBankServer/bankservermessagedispenser.h \
    couchbaseclusterdbhelper.h \
    ../RpcBankServer/Messages/bankserveractionmessagedispenser.h \
    ../RpcBankServer/Messages/bankserverbroadcastmessagedispenser.h \
    ../RpcBankServer/Messages/Actions/createbankaccountmessagedispenser.h \
    ../RpcBankServer/Messages/Broadcasts/pendingbalanceaddeddetected.h \
    ../RpcBankServer/ibankserver.h














