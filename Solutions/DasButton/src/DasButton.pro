#-------------------------------------------------
#
# Project created by QtCreator 2014-05-20T23:26:36
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = DasButton
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    dasbutton.cpp

HEADERS += \
    dasbutton.h

#molesting via old easy tree format
include(../../../GloballySharedClasses/easytreehashitem.pri)
include(../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../Projects/EasyTreeAndXmlTreeHeirarchyMolester/src/lib/LastModifiedDateHeirarchyMolester.pri)

#making new simplified last modified timestamps file (even though the pri is called easytree, it has an option for the new format)
include(../../../GloballySharedClasses/easytree.pri)
