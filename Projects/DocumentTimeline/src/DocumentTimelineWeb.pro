#-------------------------------------------------
#
# Project created by QtCreator 2015-05-26T04:13:17
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = DocumentTimelineWeb
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ui/web/

HEADERS += ui/web/documenttimelineweb.h \
    ui/web/documenttimelinewebwidget.h \
    ui/web/documenttimelinedocwebwidget.h \
    ui/web/qbytearraywmemoryresource.h \
    ui/web/documenttimelineregisterwebdialogwidget.h \
    ui/web/documenttimelineregistersubmitvideowidget.h

SOURCES += ui/web/main.cpp \
    ui/web/documenttimelineweb.cpp \
    ui/web/documenttimelinewebwidget.cpp \
    ui/web/documenttimelinedocwebwidget.cpp \
    ui/web/qbytearraywmemoryresource.cpp \
    ui/web/documenttimelineregisterwebdialogwidget.cpp \
    ui/web/documenttimelineregistersubmitvideowidget.cpp

LIBS += -lwt -lwthttp -lboost_signals #TODOreq: compile time opt into -lboost_signals2 instead of 1

include(lib/documenttimeline.pri)
include(../../../GloballySharedClasses/standardinputnotifier.pri)
