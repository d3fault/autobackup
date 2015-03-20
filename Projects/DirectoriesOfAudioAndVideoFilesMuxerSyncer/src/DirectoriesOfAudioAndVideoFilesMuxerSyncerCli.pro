#-------------------------------------------------
#
# Project created by QtCreator 2015-03-20T00:44:02
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = DirectoriesOfAudioAndVideoFilesMuxerSyncerCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += cli/directoriesofaudioandvideofilesmuxersyncercli.h

SOURCES += cli/main.cpp \
    cli/directoriesofaudioandvideofilesmuxersyncercli.cpp

include(directoriesofaudioandvideofilesmuxersyncer.pri) #i keep flip flopping on whether or not i want the pri to live in /src/ or /src/lib/. i can almost justify putting the gui.pro and cli.pro in the /src/ as well... hmmm... arbitrary but organizational decisions... yep just decided that .pro files belong next to .pri files (keeping the /src/ dir clean like that is good, no ACTUAL source files go in /src/). it additionally keeps build- dirs out of /src/, which helps me when i rsync from /src/ to my vm (sometimes the build files are sent xD)
