#-------------------------------------------------
#
# Project created by QtCreator 2014-07-10T02:31:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StevenHereAndNowGui
TEMPLATE = app


SOURCES += ui/gui/main.cpp\
        ui/gui/stevenhereandnowwidget.cpp \
    stevenhereandnow.cpp \
    bases/ievolvedmonkey.cpp \
    bases/ilawsofphysics.cpp \
    ui/gui/stevenhereandnowgui.cpp

HEADERS  += ui/gui/stevenhereandnowwidget.h \
    stevenhereandnow.h \
    bases/ievolvedmonkey.h \
    bases/ilawsofphysics.h \
    ui/gui/stevenhereandnowgui.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)
