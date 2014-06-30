#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T05:46:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesignEqualsImplementation2
TEMPLATE = app

DEFINES += DESIGNEQUALSIMPLEMENTATION_TAB "TabMotherfucker" #or 5 spaces if you're a faggot

SOURCES += main.cpp\
        designequalsimplementationmainwindow.cpp \
    designequalsimplementation.cpp \
    designequalsimplementationgui.cpp \
    designequalsimplementationproject.cpp \
    designequalsimplementationclass.cpp \
    designequalsimplementationclassslot.cpp \
    designequalsimplementationclassmethodargument.cpp \
    designequalsimplementationclassprivatemethod.cpp \
    designequalsimplementationclassproperty.cpp \
    designequalsimplementationclasssignal.cpp \
    designequalsimplementationusecase.cpp

HEADERS  += designequalsimplementationmainwindow.h \
    designequalsimplementation.h \
    designequalsimplementationgui.h \
    designequalsimplementationproject.h \
    designequalsimplementationclass.h \
    designequalsimplementationclassslot.h \
    designequalsimplementationclassmethodargument.h \
    designequalsimplementationclassprivatemethod.h \
    designequalsimplementationclassproperty.h \
    designequalsimplementationclasssignal.h \
    designequalsimplementationusecase.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)
