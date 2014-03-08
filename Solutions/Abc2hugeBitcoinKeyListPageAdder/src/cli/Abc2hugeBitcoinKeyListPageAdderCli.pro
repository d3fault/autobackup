#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T23:57:35
#
#-------------------------------------------------
#debugging this app on Debian Wheezy x86_64 requires: LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0

QT       += core

QT       -= gui

TARGET = Abc2hugeBitcoinKeyListPageAdderCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../lib/abc2hugebitcoinkeylistpageadder.h \
    abc2hugebitcoinkeylistpageaddercli.h \
    ../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h

SOURCES += main.cpp \
            ../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    ../lib/abc2hugebitcoinkeylistpageadder.cpp \
    abc2hugebitcoinkeylistpageaddercli.cpp \
    ../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp

INCLUDEPATH += ../../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../lib/
INCLUDEPATH += ../../../../GloballySharedClasses/distributeddatabase/

LIBS += -lcouchbase

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
