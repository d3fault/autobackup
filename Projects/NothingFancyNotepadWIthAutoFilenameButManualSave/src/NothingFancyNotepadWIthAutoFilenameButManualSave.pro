#-------------------------------------------------
#
# Project created by QtCreator 2014-10-08T00:06:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NothingFancyNotepadWIthAutoFilenameButManualSave
TEMPLATE = app

SOURCES += main.cpp\
        nothingfancynotepadwithautofilenamebutmanualsavemainwindow.cpp \
    newprofiledialog.cpp

HEADERS  += nothingfancynotepadwithautofilenamebutmanualsavemainwindow.h \
    newprofiledialog.h

include(../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
