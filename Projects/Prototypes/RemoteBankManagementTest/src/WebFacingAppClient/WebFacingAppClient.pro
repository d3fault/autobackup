#-------------------------------------------------
#
# Project created by QtCreator 2012-02-29T14:33:58
#
#-------------------------------------------------

QT       += core gui network

TARGET = WebFacingAppClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwebsitebankview.cpp \
    localappbankcache.cpp \
    localbankdb.cpp \
    remotebankserverhelper.cpp \
    adagencyapplogic.cpp \
    appuserdb.cpp

HEADERS  += mainwebsitebankview.h \
    localappbankcache.h \
    localbankdb.h \
    remotebankserverhelper.h \
    ../sharedProtocol/sharedprotocol.h \
    ../sharedProtocol/userBankAccount.h \
    adagencyapplogic.h \
    appuserdb.h \
    AppUserAccount.h

RESOURCES += \
    CAfiles.qrc










