#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T08:59:43
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = libAvAudioPlayer2
TEMPLATE = app


SOURCES += main.cpp\
        libavaudioplayer2test.cpp \
    libavaudioplayer2.cpp \
    iloadmediastreamsintomemory.cpp \
    curldownloader.cpp \
    libavaudiodecoder.cpp \
    synchronizer.cpp \
    qtaudioplayer.cpp

HEADERS  += libavaudioplayer2test.h \
    libavaudioplayer2.h \
    iloadmediastreamsintomemory.h \
    curldownloader.h \
    libavaudiodecoder.h \
    synchronizer.h \
    qtaudioplayer.h

LIBS += -lavcodec -lavformat -lavutil -lswscale -lz -lcurl












