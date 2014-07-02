#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T05:46:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesignEqualsImplementation
TEMPLATE = app

SOURCES += ui/gui/main.cpp\
        ui/gui/designequalsimplementationmainwindow.cpp \
    designequalsimplementation.cpp \
    ui/gui/designequalsimplementationgui.cpp \
    designequalsimplementationproject.cpp \
    designequalsimplementationclass.cpp \
    designequalsimplementationclassslot.cpp \
    designequalsimplementationclassmethodargument.cpp \
    designequalsimplementationclassprivatemethod.cpp \
    designequalsimplementationclassproperty.cpp \
    designequalsimplementationclasssignal.cpp \
    designequalsimplementationusecase.cpp \
    designequalsimplementationslotinvocationstatement.cpp \
    idesignequalsimplementationmethod.cpp \
    designequalsimplementationsignalemissionstatement.cpp \
    ui/gui/umlitemswidget.cpp \
    ui/gui/designequalsimplementationprojectaswidgetforopenedprojectstabwidget.cpp \
    ui/gui/designequalsimplementationclassdiagramaswidgetfortab.cpp \
    ui/gui/designequalsimplementationusecaseaswidgetfortab.cpp \
    ui/gui/designequalsimplementationclassasqgraphicsitemforclassdiagramscene.cpp

HEADERS  += ui/gui/designequalsimplementationmainwindow.h \
    designequalsimplementation.h \
    ui/gui/designequalsimplementationgui.h \
    designequalsimplementationproject.h \
    designequalsimplementationclass.h \
    designequalsimplementationclassslot.h \
    designequalsimplementationclassmethodargument.h \
    designequalsimplementationclassprivatemethod.h \
    designequalsimplementationclassproperty.h \
    designequalsimplementationclasssignal.h \
    designequalsimplementationusecase.h \
    idesignequalsimplementationstatement.h \
    designequalsimplementationslotinvocationstatement.h \
    idesignequalsimplementationhaveorderedlistofstatements.h \
    designequalsimplementationcommon.h \
    idesignequalsimplementationmethod.h \
    designequalsimplementationsignalemissionstatement.h \
    signalemissionorslotinvocationcontextvariables.h \
    ui/gui/umlitemswidget.h \
    ui/gui/designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h \
    ui/gui/designequalsimplementationclassdiagramaswidgetfortab.h \
    ui/gui/designequalsimplementationusecaseaswidgetfortab.h \
    ui/gui/designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)
