#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T05:34:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BulkFileTextPrependerGui
TEMPLATE = app


SOURCES += main.cpp\
        bulkfiletextprependerwidget.cpp \
    ../lib/bulkfiletextprepender.cpp \
    bulkfiletextprependergui.cpp

HEADERS  += bulkfiletextprependerwidget.h \
    ../lib/bulkfiletextprepender.h \
    bulkfiletextprependergui.h

include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
