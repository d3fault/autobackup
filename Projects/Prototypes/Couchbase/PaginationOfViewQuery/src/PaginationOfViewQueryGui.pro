#-------------------------------------------------
#
# Project created by QtCreator 2014-11-04T23:49:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PaginationOfViewQueryGui
TEMPLATE = app

SOURCES += main.cpp\
        paginationofviewquerywidget.cpp \
    paginationofviewquerygui.cpp \
    paginationofviewquery.cpp \
    ../../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp

HEADERS  += paginationofviewquerywidget.h \
    paginationofviewquerygui.h \
    paginationofviewquery.h \
    ../../../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h \
    paginationofviewquerycommon.h

INCLUDEPATH += ../../../../../GloballySharedClasses/distributeddatabase/

include(../../../../../GloballySharedClasses/objectonthreadgroup.pri)

LIBS += -lcouchbase
