#-------------------------------------------------
#
# Project created by QtCreator 2014-10-24T18:42:09
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = NotifyGitBitShitDaemonOfMessage
#was called NotifyGitBitShitDaemonOfInternalMetaRepoUpdate

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    notifygitbitshitdaemonofmessagecli.cpp

HEADERS += \
    ../../GitBitShitShared/gitbitshitshared.h \
    notifygitbitshitdaemonofmessagecli.h

INCLUDEPATH += ../../GitBitShitShared/
