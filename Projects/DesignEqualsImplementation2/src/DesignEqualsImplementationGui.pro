#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T05:46:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

contains(QT_CONFIG, opengl): QT += opengl

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
    ui/gui/designequalsimplementationprojectaswidgetforopenedprojectstabwidget.cpp \
    ui/gui/designequalsimplementationclassdiagramaswidgetfortab.cpp \
    ui/gui/designequalsimplementationusecaseaswidgetfortab.cpp \
    ui/gui/designequalsimplementationclassasqgraphicsitemforclassdiagramscene.cpp \
    ui/gui/classdiagramgraphicsscene.cpp \
    ui/gui/ihaveagraphicsviewandscene.cpp \
    ui/gui/usecasegraphicsscene.cpp \
    ui/gui/idesignequalsimplementationgraphicsscene.cpp \
    ui/gui/usecaseumlitemswidget.cpp \
    ui/gui/classdiagramumlitemswidget.cpp \
    ui/gui/designequalsimplementationclasslifelinegraphicsitemforusecasescene.cpp \
    designequalsimplementationclasslifeline.cpp \
    designequalsimplementationclasslifelineunitofexecution.cpp \
    ui/gui/designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.cpp \
    ui/gui/designequalsimplementationactorgraphicsitemforusecasescene.cpp \
    designequalsimplementationactor.cpp \
    ui/gui/signalslotconnectionactivationarrowforgraphicsscene.cpp \
    ui/gui/slotinvocationdialog.cpp

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
    ui/gui/designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h \
    ui/gui/designequalsimplementationclassdiagramaswidgetfortab.h \
    ui/gui/designequalsimplementationusecaseaswidgetfortab.h \
    ui/gui/designequalsimplementationclassasqgraphicsitemforclassdiagramscene.h \
    ui/gui/designequalsimplementationguicommon.h \
    idesignequalsimplementationvisuallyrepresenteditem.h \
    ui/gui/classdiagramgraphicsscene.h \
    ui/gui/ihaveagraphicsviewandscene.h \
    ui/gui/usecasegraphicsscene.h \
    ui/gui/idesignequalsimplementationgraphicsscene.h \
    ui/gui/usecaseumlitemswidget.h \
    ui/gui/classdiagramumlitemswidget.h \
    ui/gui/designequalsimplementationclasslifelinegraphicsitemforusecasescene.h \
    designequalsimplementationclasslifeline.h \
    designequalsimplementationclasslifelineunitofexecution.h \
    ui/gui/designequalsimplementationclasslifelineunitofexecutiongraphicsitemforusecasescene.h \
    ui/gui/designequalsimplementationactorgraphicsitemforusecasescene.h \
    designequalsimplementationactor.h \
    ui/gui/signalslotconnectionactivationarrowforgraphicsscene.h \
    ui/gui/slotinvocationdialog.h \
    ihavetypeandvariablenameandpreferredtextualrepresentation.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)