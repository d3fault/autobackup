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

DEFINES += NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE=100

INCLUDEPATH += ui/web/

SOURCES += ui/web/main.cpp \
    ui/web/documenttimelineweb.cpp \
    ui/web/documenttimelinewebwidget.cpp \
    ui/web/documenttimelinedocwebwidget.cpp \
    ui/web/qbytearraywmemoryresource.cpp

include(lib/documenttimeline.pri)

HEADERS += ui/web/documenttimelineweb.h \
    ui/web/documenttimelinewebwidget.h \
    ui/web/documenttimelinedocwebwidget.h \
    ui/web/qbytearraywmemoryresource.h

LIBS += -lwt -lwthttp

include(../../../GloballySharedClasses/standardinputnotifier.pri)
