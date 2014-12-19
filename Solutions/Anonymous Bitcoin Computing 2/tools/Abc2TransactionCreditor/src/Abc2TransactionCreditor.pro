#-------------------------------------------------
#
# Project created by QtCreator 2014-12-17T23:50:19
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = Abc2TransactionCreditor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    ../../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h \
    abc2transactioncreditor.h

SOURCES += main.cpp \
    ../../../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    ../../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp \
    abc2transactioncreditor.cpp

INCLUDEPATH += ../../../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../../../GloballySharedClasses/distributeddatabase/

LIBS += -lcouchbase
