#-------------------------------------------------
#
# Project created by QtCreator 2013-06-18T05:39:09
#
#-------------------------------------------------

QT       += core gui

TARGET = LifeShaperStaticFiles
TEMPLATE = app


SOURCES += main.cpp\
        lifeshaperstaticfileswidget.cpp \
    lifeshaperstaticfilestest.cpp \
    lifeshaperstaticfilesbusiness.cpp \
    lifeshaperstaticfiles.cpp

HEADERS  += lifeshaperstaticfileswidget.h \
    lifeshaperstaticfilestest.h \
    lifeshaperstaticfilesbusiness.h \
    lifeshaperstaticfiles.h

include(../../../../GloballySharedClasses/easytreeparser.pri)
include(../../../../GloballySharedClasses/easytreehashitem.pri)
include(../../../../GloballySharedClasses/objectonthreadhelper.pri)
