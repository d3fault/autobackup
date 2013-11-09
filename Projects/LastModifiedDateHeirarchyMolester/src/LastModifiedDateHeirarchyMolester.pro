#-------------------------------------------------
#
# Project created by QtCreator 2013-11-09T00:18:56
#
#-------------------------------------------------

QT       += core gui

TARGET = LastModifiedDateHeirarchyMolester
TEMPLATE = app


SOURCES += main.cpp\
        lastmodifieddateheirarchymolesterwidget.cpp \
    lastmodifieddateheirarchymolestergui.cpp \
    lastmodifieddateheirarchymolester.cpp \
    filemodificationdatechanger.cpp

HEADERS  += lastmodifieddateheirarchymolesterwidget.h \
    lastmodifieddateheirarchymolestergui.h \
    lastmodifieddateheirarchymolester.h \
    filemodificationdatechanger.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/easytreehashitem.pri)
