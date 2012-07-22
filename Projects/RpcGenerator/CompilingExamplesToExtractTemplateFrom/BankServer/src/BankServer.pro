#-------------------------------------------------
#
# Project created by QtCreator 2012-07-18T03:17:30
#
#-------------------------------------------------

QT       += core gui network

TARGET = BankServer
TEMPLATE = app


SOURCES += main.cpp\
        bankserverdebugwidget.cpp \
    idebuggablestartablestoppablebankserverbackend.cpp \
    bankservertest.cpp \
    bankserver.cpp \
    bitcoinhelper.cpp \
    AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.cpp \
    AutoGeneratedRpcBankServerCode/ssltcpserverandbankserverprotocolknower.cpp \
    AutoGeneratedRpcBankServerCode/ibankserverprotocolknower.cpp \
    AutoGeneratedRpcBankServerCode/irpcbankserverbusiness.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/imessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/createbankaccountmessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/getaddfundskeymessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispenser.cpp \    
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.cpp \
    AutoGeneratedRpcBankServerCode/ssltcpserver.cpp \
    bankdbhelper.cpp \
    AutoGeneratedRpcBankServerCode/iemitrpcbankserveractionrequestsignalswithmessageasparam.cpp

HEADERS  += bankserverdebugwidget.h \
    idebuggablestartablestoppablebankserverbackend.h \
    bankservertest.h \
    bankserver.h \
    bitcoinhelper.h \
    AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.h \
    AutoGeneratedRpcBankServerCode/ssltcpserverandbankserverprotocolknower.h \
    AutoGeneratedRpcBankServerCode/ibankserverprotocolknower.h \
    AutoGeneratedRpcBankServerCode/irpcbankserverbusiness.h \
    ../../RpcBankServerAndClientShared/rpcbankserverheader.h \   
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/imessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/pendingbalancedetectedmessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Broadcasts/confirmedbalancedetectedmessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/createbankaccountmessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/Actions/getaddfundskeymessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h \
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h \
    AutoGeneratedRpcBankServerCode/ssltcpserver.h \
    bankdbhelper.h \
    AutoGeneratedRpcBankServerCode/iemitrpcbankserveractionrequestsignalswithmessageasparam.h





