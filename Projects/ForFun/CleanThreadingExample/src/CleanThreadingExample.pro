QT       += core gui

TARGET = CleanThreadingExample
TEMPLATE = app


SOURCES += main.cpp\
        cleanthreadingfrontendwidget.cpp \
    cleanthreadingexampletest.cpp \
    cleanthreadingbackend1.cpp \
    cleanthreadingbackend2.cpp

HEADERS  += cleanthreadingfrontendwidget.h \
    cleanthreadingexampletest.h \
    objectonthreadhelper.h \
    cleanthreadingbackend1.h \
    cleanthreadingbackend2.h
