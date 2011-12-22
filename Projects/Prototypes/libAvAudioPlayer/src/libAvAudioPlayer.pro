#-------------------------------------------------
#
# Project created by QtCreator 2011-12-22T04:21:23
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = libAvAudioPlayer
TEMPLATE = app


SOURCES += main.cpp\
        libavaudioplayerwidget.cpp \
    libavaudioplayer.cpp

HEADERS  += libavaudioplayerwidget.h \
    libavaudioplayer.h

LIBS += -lavcodec -lavformat -lavutil -lswscale -lz

