#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T21:30:50
#
#-------------------------------------------------
#debugging this app on Debian Wheezy x86_64 requires: LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0

QT       += core

QT       -= gui

TARGET = Abc2dbInitializerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    ../lib/abc2dbinitializer.h \
    ../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    abc2dbinitializercli.h \
    ../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h

SOURCES += main.cpp \
    ../lib/abc2dbinitializer.cpp \
    ../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    abc2dbinitializercli.cpp \
    ../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp

INCLUDEPATH += ../../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../../GloballySharedClasses/distributeddatabase/
INCLUDEPATH += ../lib/

LIBS += -lcouchbase

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
