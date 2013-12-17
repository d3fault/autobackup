#-------------------------------------------------
#
# Project created by QtCreator 2013-12-15T21:02:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

TARGET = ConcurrentLossyDirectoryCopier
TEMPLATE = app


SOURCES += main.cpp\
        concurrentlossydirectorycopierwidget.cpp \
    concurrentlossydirectorycopier.cpp \
    concurrentlossydirectorycopiergui.cpp

HEADERS  += concurrentlossydirectorycopierwidget.h \
    concurrentlossydirectorycopier.h \
    copyoperation.h \
    concurrentlossydirectorycopiergui.h

include(/home/d3fault/autobackupRO/GloballySharedClasses/objectonthreadhelper.pri)
