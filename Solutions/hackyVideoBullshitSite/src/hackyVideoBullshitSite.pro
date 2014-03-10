TARGET = HackyVideoBullshitSite

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    hackyvideobullshitsite.cpp \
    adimagegetandsubscribemanager.cpp

HEADERS += \
    hackyvideobullshitsite.h \
    adimagegetandsubscribemanager.h \
    ../../../GloballySharedClasses/threading/boostthreadinitializationsynchronizationkit.h

INCLUDEPATH += ../../../GloballySharedClasses/threading/
