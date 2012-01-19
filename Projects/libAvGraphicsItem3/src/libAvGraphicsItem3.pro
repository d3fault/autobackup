#-------------------------------------------------
#
# Project created by QtCreator 2012-01-19T12:58:25
#
#-------------------------------------------------

QT       += core gui multimedia

contains(QT_CONFIG, opengl): QT += opengl

TARGET = libAvGraphicsItem3
TEMPLATE = app


SOURCES += main.cpp \
    frontend/libavgraphicsitem3test.cpp \
    frontend/videoplayerwidget.cpp \
    backend/libavplayer.cpp \
    frontend/videographicsitem.cpp

HEADERS  += \
    frontend/libavgraphicsitem3test.h \
    frontend/videoplayerwidget.h \
    backend/libavplayer.h \
    frontend/videographicsitem.h










