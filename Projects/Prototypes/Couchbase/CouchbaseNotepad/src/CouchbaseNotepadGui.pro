#-------------------------------------------------
#
# Project created by QtCreator 2014-10-11T16:11:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CouchbaseNotepadGui
TEMPLATE = app


SOURCES += main.cpp\
        couchbasenotepadwidget.cpp \
    couchbasenotepadgui.cpp \
    couchbasenotepad.cpp

HEADERS  += couchbasenotepadwidget.h \
    couchbasenotepadgui.h \
    couchbasenotepad.h

include(../../../../../GloballySharedClasses/objectonthreadgroup.pri)

LIBS += -lcouchbase -levent
