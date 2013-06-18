#-------------------------------------------------
#
# Project created by QtCreator 2013-06-18T02:12:09
#
#-------------------------------------------------

QT       += core gui

TARGET = gitSubmoduleAutoBackupHalper
TEMPLATE = app


SOURCES += main.cpp\
        gitsubmoduleautobackuphalperwidget.cpp \
    gitsubmoduleautobackuphalper.cpp \
    gitsubmoduleautobackuphalperbusiness.cpp

HEADERS  += gitsubmoduleautobackuphalperwidget.h \
    gitsubmoduleautobackuphalper.h \
    gitsubmoduleautobackuphalperbusiness.h

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
