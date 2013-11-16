#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T01:49:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GitUnrollRerollCensorshipMachineCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    gitunrollrerollcensorshipmachinecli.cpp

HEADERS += \
    gitunrollrerollcensorshipmachinecli.h

include(../lib/gitunrollrerollcensorshipmachine.pri)
include(../../../../GloballySharedClasses/easytreeparser.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)
include(../../../../GloballySharedClasses/githelper.pri)

#ifdef hacks, whatever
include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../LastModifiedDateHeirarchyMolester/src/lib/LastModifiedDateHeirarchyMolester.pri)
#bah I need my auto git submodules design. that + qbs and all this stupidity (I was worndering whether or not to move LastModifiedDateHeirarchyMolester to GloballyShared, but meh) should go away...
