#-------------------------------------------------
#
# Project created by QtCreator 2013-12-17T02:03:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimplifiedHeirarchyMolester
TEMPLATE = app


SOURCES += main.cpp\
        simplifiedheirarchymolesterwidget.cpp \
    simplifiedheirarchymolester.cpp \
    simplifiedheirarchymolestergui.cpp

HEADERS  += simplifiedheirarchymolesterwidget.h \
    simplifiedheirarchymolester.h \
    simplifiedheirarchymolestergui.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
include(../../../GloballySharedClasses/simplifiedlastmodifiedtimestamp.pri)
