TARGET = HackyVideoBullshitSite

QT += core network
QT -= gui
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

#CONFIG += no_keywords #bah, wt is great except it's GPL and fucks with my signals/slots
#DEFINES += QT_NO_KEYWORDS

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
    backend/videosegmentsimporterfolderwatcher.h \
    hackyvideobullshitsitebackendscopeddeleter.h

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/image/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/

LIBS += -lwt -lwthttp -lboost_signals

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
