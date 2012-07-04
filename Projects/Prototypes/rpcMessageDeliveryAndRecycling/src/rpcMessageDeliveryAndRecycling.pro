#-------------------------------------------------
#
# Project created by QtCreator 2012-07-01T22:06:56
#
#-------------------------------------------------

QT       += core gui

TARGET = rpcMessageDeliveryAndRecycling
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    rpcclientshelperanddeliveryacceptorandnetwork.cpp \
    rpcserverbusinessimpl.cpp \
    bitcoinhelperandbroadcastmessagedispenseruser.cpp \
    messageDispensers/broadcasts/pendingbalanceaddeddetectedmessagedispenser.cpp \
    messages/broadcasts/pendingbalanceaddeddetectedmessage.cpp \
    messageDispensers/actions/createbankaccountmessagedispenser.cpp \
    messages/actions/createbankaccountmessage.cpp

HEADERS  += mainwidget.h \
    rpcclientshelperanddeliveryacceptorandnetwork.h \
    rpcserverbusinessimpl.h \
    bitcoinhelperandbroadcastmessagedispenseruser.h \
    irpcserverimpl.h \
    irpcclientshelper.h \
    messageDispensers/broadcastdispensers.h \
    messageDispensers/broadcasts/pendingbalanceaddeddetectedmessagedispenser.h \
    messageDispensers/imessagedispenser.h \
    messages/imessage.h \
    messages/broadcasts/pendingbalanceaddeddetectedmessage.h \
    messageDispensers/actiondispensers.h \
    messageDispensers/actions/createbankaccountmessagedispenser.h \
    messages/actions/createbankaccountmessage.h




























