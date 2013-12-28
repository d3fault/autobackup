#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T08:41:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RecursiveFileStringReplaceRenamerGui
TEMPLATE = app


SOURCES += main.cpp\
        recursivefilestringreplacerenamerwidget.cpp \
    ../lib/recursivefilestringreplacerenamer.cpp \
    recursivefilestringreplacerenamergui.cpp

HEADERS  += recursivefilestringreplacerenamerwidget.h \
    ../lib/recursivefilestringreplacerenamer.h \
    recursivefilestringreplacerenamergui.h

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
