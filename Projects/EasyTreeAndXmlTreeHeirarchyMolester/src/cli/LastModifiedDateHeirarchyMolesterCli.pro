#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T00:19:07
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LastModifiedDateHeirarchyMolesterCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lastmodifieddateheirarchymolestercli.cpp

HEADERS += \
    lastmodifieddateheirarchymolestercli.h

#TODOreq: should it be /lib/libLastModi[...]?
include(../lib/LastModifiedDateHeirarchyMolester.pri)

include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)

#TODOreq: correct solution to pri dependency hell? (pri files can't include pri files, because if you include two pri files that include the same sub-pri file, that breaks compilation -_-). I'd move to qbs in a heartbeat if I knew it solved it (hell, it might :-P -- PROBABLY DOES. I'm just not as comfortable with it)
