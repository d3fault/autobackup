#-------------------------------------------------
#
# Project created by QtCreator 2014-11-30T22:42:41
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = Abc2WithdrawRequestProcessorCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    abc2withdrawrequestprocessorcli.cpp \
    abc2withdrawrequestprocessor.cpp \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp

HEADERS += \
    abc2withdrawrequestprocessorcli.h \
    abc2withdrawrequestprocessor.h \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/

LIBS += -lcouchbase
