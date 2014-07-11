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
    ui/gui/stevenhereandnowgui.cpp \
    inheritedtraits/file.cpp \
    inheritedtraits/iinheritedtrait.cpp \
    inheritedtraits/tagcloud.cpp \
    bases/imustbeunderstood.cpp \
    inheritedtraits/brain.cpp \
    inheritedtraits/ilogicprocessor.cpp \
    inheritedtraits/computer.cpp

HEADERS  += ui/gui/stevenhereandnowwidget.h \
    stevenhereandnow.h \
    bases/ievolvedmonkey.h \
    bases/ilawsofphysics.h \
    ui/gui/stevenhereandnowgui.h \
    bases/imustbeunderstood.h \
    bases/ilogic.h \
    inheritedtraits/file.h \
    inheritedtraits/iinheritedtrait.h \
    inheritedtraits/tagcloud.h \
    inheritedtraits/brain.h \
    inheritedtraits/ilogicprocessor.h \
    inheritedtraits/computer.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)
