#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T10:31:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CopyFileContentsToClipboardGui
TEMPLATE = app

SOURCES += main.cpp \
    copyfilecontentstoclipboardwidget.cpp

HEADERS  += \
    copyfilecontentstoclipboardwidget.h

include(../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
