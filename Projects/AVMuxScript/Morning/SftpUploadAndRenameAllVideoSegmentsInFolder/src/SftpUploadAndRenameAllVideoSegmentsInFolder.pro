#-------------------------------------------------
#
# Project created by QtCreator 2015-03-10T13:01:10
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = SftpUploadAndRenameAllVideoSegmentsInFolder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    lib/sftpuploadandrenameallvideosegmentsinfolder.cpp

include(../../../../SftpUploaderAndRenamerQueue/src/sftpuploadandrenamerqueue.pri)

HEADERS += \
    lib/sftpuploadandrenameallvideosegmentsinfolder.h
