#-------------------------------------------------
#
# Project created by QtCreator 2013-11-09T00:18:56
#
#-------------------------------------------------

QT       += core gui

TARGET = LastModifiedDateHeirarchyMolesterGui
TEMPLATE = app


SOURCES += main.cpp\
        lastmodifieddateheirarchymolesterwidget.cpp \
    lastmodifieddateheirarchymolestergui.cpp

HEADERS  += lastmodifieddateheirarchymolesterwidget.h \
    lastmodifieddateheirarchymolestergui.h

include(../lib/LastModifiedDateHeirarchyMolester.pri)

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)
