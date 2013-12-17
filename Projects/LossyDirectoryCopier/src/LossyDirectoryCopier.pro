#-------------------------------------------------
#
# Project created by QtCreator 2013-12-08T23:33:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LossyDirectoryCopier
TEMPLATE = app


SOURCES += main.cpp\
        lossydirectorycopierwidget.cpp \
    lossydirectorycopiergui.cpp \
    lossydirectorycopier.cpp

HEADERS  += lossydirectorycopierwidget.h \
    lossydirectorycopiergui.h \
    lossydirectorycopier.h

include(/home/d3fault/autobackupRO/GloballySharedClasses/objectonthreadhelper.pri)
