#-------------------------------------------------
#
# Project created by QtCreator 2015-03-10T13:01:10
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = SftpUploadAndRenameAllVideoSegmentsInFolderCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += cli/sftpuploadandrenameallvideosegmentsinfoldercli.h

SOURCES += cli/main.cpp \
    cli/sftpuploadandrenameallvideosegmentsinfoldercli.cpp

include(../../../../SftpUploaderAndRenamerQueue/src/sftpuploadandrenamerqueue.pri)
include(sftpuploadandrenameallvideosegmentsinfolder.pri)
