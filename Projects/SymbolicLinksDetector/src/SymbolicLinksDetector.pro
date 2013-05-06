#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T03:13:52
#
#-------------------------------------------------

QT       += core gui

TARGET = SymbolicLinksDetector
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    symboliclinksdetectorgui.cpp \
    symboliclinksdetectorbusiness.cpp \
    symboliclinksdetector.cpp

HEADERS  += mainwidget.h \
    symboliclinksdetectorgui.h \
    symboliclinksdetectorbusiness.h \
    symboliclinksdetector.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
