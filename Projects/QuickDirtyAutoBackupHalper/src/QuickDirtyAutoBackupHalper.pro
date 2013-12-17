#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T01:02:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuickDirtyAutoBackupHalper
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    quickdirtyautobackuphalper.cpp \
    quickdirtyautobackuphalperbusiness.cpp \

HEADERS  += mainwidget.h \
    quickdirtyautobackuphalper.h \
    quickdirtyautobackuphalperbusiness.h \

include(../../../GloballySharedClasses/easytree.pri)
