#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T11:04:38
#
#-------------------------------------------------

QT       += core gui

TARGET = GitUnrollRerollCensorshipMachineGui
TEMPLATE = app


SOURCES += main.cpp\
        gitunrollrerollcensorshipmachinewidget.cpp \
    gitunrollrerollcensorshipmachinegui.cpp

HEADERS  += gitunrollrerollcensorshipmachinewidget.h \
    gitunrollrerollcensorshipmachinegui.h

include(../lib/gitunrollrerollcensorshipmachine.pri)
include(../../../../GloballySharedClasses/easytreeparser.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)
include(../../../../GloballySharedClasses/githelper.pri)
include(../../../../GloballySharedClasses/objectonthreadhelper.pri)

include(../../../../GloballySharedClasses/filemodificationdatechanger.pri)
include(../../../LastModifiedDateHeirarchyMolester/src/lib/LastModifiedDateHeirarchyMolester.pri)
include(../../../../GloballySharedClasses/easytree.pri)
