#debugging this app on Debian Wheezy x86_64 requires: LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0
TARGET = Abc2getTodaysAdSlot

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h \
    gettodaysadslotwresource.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../../../GloballySharedClasses/distributeddatabase/d3faultscouchbaseshared.h \
    synchronoustodaysadslotgetter.h

SOURCES += main.cpp \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp \
    gettodaysadslotwresource.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    synchronoustodaysadslotgetter.cpp

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/

LIBS += -lcouchbase -lwt -lwthttp
