TARGET = HackyVideoBullshitSite

QT += core network
QT -= gui
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += no_keywords #bah

DEFINES += QT_NO_KEYWORDS \
    SHARED_VIDEO_SEGMENTS_ARRAY_SIZE 1000000 #(((1000000*64)/8)*3)/60/24/365 = 45.66210 days before i run out, with only the cost of 8mb :-P, 4mb on 32-bit platforms (excluding the size of the strings they point to, which will be all over the place and allocated as needed xD)


SOURCES += main.cpp \
    hackyvideobullshitsite.cpp \
    backend/adimagegetandsubscribemanager.cpp \
    frontend/hackyvideobullshitsitegui.cpp \
    backend/adimagewresource.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    backend/hackyvideobullshitsitebackend.cpp \
    backend/videosegmentsimporterfolderwatcher.cpp

HEADERS += \
    hackyvideobullshitsite.h \
    backend/adimagegetandsubscribemanager.h \
    frontend/hackyvideobullshitsitegui.h \
    backend/adimagewresource.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../../../GloballySharedClasses/image/nonanimatedimagheaderchecker.h \
    ../../../GloballySharedClasses/distributeddatabase/d3faultscouchbaseshared.h \
    backend/hackyvideobullshitsitebackend.h \
    backend/videosegmentsimporterfolderwatcher.h

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/image/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/

LIBS += -lwt -lwthttp -lboost_signals

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
