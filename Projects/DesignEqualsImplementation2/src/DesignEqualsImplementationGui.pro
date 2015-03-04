#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T05:46:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#contains(QT_CONFIG, opengl): QT += opengl
DEFINES += QT_NO_OPENGL #comment this out and uncomment above line for OpenGL

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
    ui/gui/designequalsimplementationactorgraphicsitemforusecasescene.cpp \
    designequalsimplementationactor.cpp \
    ui/gui/signalslotconnectionactivationarrowforgraphicsscene.cpp \
    ui/gui/signalslotmessagedialog.cpp \
    ui/gui/snappingindicationvisualrepresentation.cpp \
    idesignequalsimplementationhaveorderedlistofstatements.cpp \
    designequalsimplementationsignalslotinterface.cpp \
    ui/gui/classinstancechooserdialog.cpp \
    ui/gui/sourceslotsnappingindicationvisualrepresentation.cpp \
    ui/gui/destinationslotsnappingindicationvisualrepresentation.cpp \
    hasa_private_classes_member.cpp \
    designequalsimplementationprojectgenerator.cpp \
    ui/gui/classeditordialog.cpp \
    ui/gui/methodsingleargumentwidget.cpp \
    ui/gui/tabemittinglineedit.cpp \
    ui/gui/newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.cpp \
    designequalsimplementationlenientsignalorslotsignaturerparser.cpp \
    designequalsimplementationlenientpropertydeclarationparser.cpp \
    designequalsimplementationprivatemethodsynchronouscallstatement.cpp \
    idesignequalsimplementationstatement.cpp \
    signalemissionorslotinvocationcontextvariables.cpp \
    designequalsimplementationprojectserializer.cpp \
    ui/gui/renameprojectdialog.cpp \
    ui/gui/designequalsimplementationexistinsignalgraphicsitemforusecasescene.cpp \
    ui/gui/designequalsimplementationslotinvokegraphicsitemforusecasescene.cpp \
    ui/gui/designequalsimplementationprivatemethodinvokestatementgraphicsitemforusecasescene.cpp \
    ui/gui/designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.cpp \
    ui/gui/sourceexistingsignalsnappingindicationvisualrepresentation.cpp \
    ui/gui/signalstatementnotchmultiplextergraphicsrect.cpp \
    designequalsimplementationchunkofrawcppstatements.cpp

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
    ui/gui/designequalsimplementationactorgraphicsitemforusecasescene.h \
    designequalsimplementationactor.h \
    ui/gui/signalslotconnectionactivationarrowforgraphicsscene.h \
    ihavetypeandvariablenameandpreferredtextualrepresentation.h \
    ui/gui/signalslotmessagedialog.h \
    ui/gui/snappingindicationvisualrepresentation.h \
    designequalsimplementationsignalslotinterface.h \
    ui/gui/classinstancechooserdialog.h \
    ui/gui/sourceslotsnappingindicationvisualrepresentation.h \
    ui/gui/destinationslotsnappingindicationvisualrepresentation.h \
    hasa_private_classes_member.h \
    designequalsimplementationprojectgenerator.h \
    ui/gui/classeditordialog.h \
    ui/gui/methodsingleargumentwidget.h \
    ui/gui/tabemittinglineedit.h \
    ui/gui/newtypeseen_createdesignequalsclassfromit_ornoteasdefinedelsewheretype_dialog.h \
    designequalsimplementationlenientsignalorslotsignaturerparser.h \
    designequalsimplementationlenientpropertydeclarationparser.h \
    designequalsimplementationprivatemethodsynchronouscallstatement.h \
    designequalsimplementationprojectserializer.h \
    ui/gui/renameprojectdialog.h \
    ui/gui/designequalsimplementationexistinsignalgraphicsitemforusecasescene.h \
    ui/gui/designequalsimplementationslotinvokegraphicsitemforusecasescene.h \
    ui/gui/designequalsimplementationprivatemethodinvokestatementgraphicsitemforusecasescene.h \
    ui/gui/designequalsimplementationclasslifelineslotgraphicsitemforusecasescene.h \
    ui/gui/isnappablesourcegraphicsitem.h \
    ui/gui/sourceexistingsignalsnappingindicationvisualrepresentation.h \
    ui/gui/signalstatementnotchmultiplextergraphicsrect.h \
    designequalsimplementationchunkofrawcppstatements.h

#libclang stuff
CONFIG += precompile_header
PRECOMPILED_HEADER = libclangpch.h
DEFINES += NDEBUG
DEFINES += _GNU_SOURCE
DEFINES += __STDC_CONSTANT_MACROS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += __STDC_LIMIT_MACROS
INCLUDEPATH += "/usr/lib/llvm-3.4/include"
LIBS += -L/usr/lib/llvm-3.4/lib -lclangTooling -lclangDriver -lclangFrontend -lclangParse -lclangSema -lclangEdit -lclangAnalysis -lclangLex -lclangSerialization -lclangAST -lclangBasic -lclang
