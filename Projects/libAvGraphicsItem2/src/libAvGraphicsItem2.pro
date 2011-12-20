#-------------------------------------------------
#
# Project created by QtCreator 2011-12-18T01:00:42
#
#-------------------------------------------------

QT       += core gui multimedia

contains(QT_CONFIG, opengl): QT += opengl

TARGET = libAvGraphicsItem2
TEMPLATE = app


SOURCES += main.cpp\
        frontend/libavplayerwidget.cpp \
	backend/libavplayerbackend.cpp \
    backend/libavdecoder.cpp \
    backend/audiovideosynchronizer.cpp \
    frontend/libavgraphicsitem.cpp \
    frontend/rawaudioplayer.cpp

HEADERS  += frontend/libavplayerwidget.h \
    backend/libavplayerbackend.h \
    backend/libavdecoder.h \
    backend/audiovideosynchronizer.h \
    frontend/libavgraphicsitem.h \
    frontend/rawaudioplayer.h

#libav includes
win32:INCLUDEPATH += "C:/libav"

#libav dynamic linking
win32:LIBS += -L"C:/libav/libavcodec" -lavcodec
win32:LIBS += -L"C:/libav/libavformat" -lavformat
win32:LIBS += -L"C:/libav/libavutil" -lavutil
win32:LIBS += -L"C:/libav/libswscale" -lswscale

LIBS += -lavcodec -lavformat -lavutil -lswscale -lz










