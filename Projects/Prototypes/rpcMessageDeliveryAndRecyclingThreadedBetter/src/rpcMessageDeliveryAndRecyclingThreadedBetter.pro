#-------------------------------------------------
#
# Project created by QtCreator 2012-07-06T15:20:38
#
#-------------------------------------------------

QT       += core gui

TARGET = rpcMessageDeliveryAndRecyclingThreadedBetter
TEMPLATE = app


SOURCES += main.cpp\
    rpcmessagedeliveryandrecyclingthreadedbettertest.cpp \
    debugstartstopgui.cpp \
    idebuggablestartablestoppablebackend.cpp \
    bitcoinhelper.cpp \
    messageDispensers/broadcastdispensers.cpp \
    messageDispensers/imessagedispenser.cpp \
    messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.cpp \
    irpcbankserverclientshelper.cpp \
    irpcbankserver.cpp \
    rpcbankserverimpl.cpp \
    rpcbankserverclientshelperimpl.cpp \
    irpcbankservermessagetransporter.cpp \
    ssltcpserverandprotocolknower.cpp \
    messages/imessage.cpp \
    messages/actions/createbankaccountmessage.cpp \
    messageDispensers/actions/createbankaccountmessagedispenser.cpp \
    messages/broadcasts/pendingbalanceaddedmessage.cpp

HEADERS  += \
    rpcmessagedeliveryandrecyclingthreadedbettertest.h \
    debugstartstopgui.h \
    idebuggablestartablestoppablebackend.h \
    bitcoinhelper.h \
    messageDispensers/broadcastdispensers.h \
    messageDispensers/imessagedispenser.h \
    messageDispensers/broadcasts/pendingbalanceaddedmessagedispenser.h \
    irpcbankserverclientshelper.h \
    irpcbankserver.h \
    rpcbankserverimpl.h \
    rpcbankserverclientshelperimpl.h \
    irpcbankservermessagetransporter.h \
    ssltcpserverandprotocolknower.h \
    messages/imessage.h \
    messages/actions/createbankaccountmessage.h \
    messageDispensers/actions/createbankaccountmessagedispenser.h \
    messages/broadcasts/pendingbalanceaddedmessage.h
































































