#-------------------------------------------------
#
# Project created by QtCreator 2014-07-29T18:30:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleAsyncLibrarySlotInvokeAndSignalResponseGui
TEMPLATE = app


SOURCES += ui/gui/main.cpp\
        ui/gui/simpleasynclibraryslotinvokeandsignalresponsewidget.cpp \
    ui/gui/simpleasynclibraryslotinvokeandsignalresponsegui.cpp \
    bar.cpp \
    foo.cpp

HEADERS  += ui/gui/simpleasynclibraryslotinvokeandsignalresponsewidget.h \
    ui/gui/simpleasynclibraryslotinvokeandsignalresponsegui.h \
    foo.h \
    bar.h

include(../../../../../../GloballySharedClasses/objectonthreadgroup.pri)
