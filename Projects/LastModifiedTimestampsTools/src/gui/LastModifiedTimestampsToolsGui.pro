#-------------------------------------------------
#
# Project created by QtCreator 2013-12-17T02:03:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LastModifiedTimestampsToolsGui
TEMPLATE = app


SOURCES += main.cpp\
    ../lib/lastmodifiedtimestampstools.cpp \
    lastmodifiedtimestampstoolswidget.cpp \
    lastmodifiedtimestampstoolsgui.cpp

HEADERS  += \
    ../lib/lastmodifiedtimestampstools.h \
    lastmodifiedtimestampstoolswidget.h \
    lastmodifiedtimestampstoolsgui.h

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../GloballySharedClasses/lastmodifiedtimestamp.pri)
include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
