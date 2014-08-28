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
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    backend/videosegmentsimporterfolderwatcher.cpp \
    frontend/directorybrowsingwtwidget.cpp \
    frontend/timelinewtwidget.cpp \
    backend/lastmodifiedtimestampswatcher.cpp \
    ../../../Projects/LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.cpp \
    wtcontrollerandstdoutowner.cpp \
    ../../../Projects/SftpUploaderAndRenamerQueue/src/lib/sftpuploaderandrenamerqueue.cpp \
    backend/nonexpiringstringwresource.cpp

HEADERS += \
    hackyvideobullshitsite.h \
    backend/adimagegetandsubscribemanager.h \
    frontend/hackyvideobullshitsitegui.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../../../GloballySharedClasses/image/nonanimatedimagheaderchecker.h \
    backend/videosegmentsimporterfolderwatcher.h \
    frontend/directorybrowsingwtwidget.h \
    frontend/timelinewtwidget.h \
    backend/lastmodifiedtimestampswatcher.h \
    ../../../Projects/LastModifiedTimestampsSorter/src/lib/lastmodifiedtimestampssorter.h \
    lastmodifiedtimestampsandpaths.h \
    wtcontrollerandstdoutowner.h \
    frontend/hvbsshared.h \
    ../../../Projects/SftpUploaderAndRenamerQueue/src/lib/sftpuploaderandrenamerqueue.h \
    backend/nonexpiringstringwresource.h

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/image/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/
INCLUDEPATH += ../../../Projects/LastModifiedTimestampsSorter/src/lib/
INCLUDEPATH += ../../../Projects/SftpUploaderAndRenamerQueue/src/lib/

LIBS += -lwt -lwthttp -lboost_signals

include(../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../GloballySharedClasses/lastmodifiedtimestamp.pri)
include(../../../GloballySharedClasses/standardinputnotifier.pri)

RESOURCES += \
    hvbsresources.qrc
