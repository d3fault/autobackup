QT       += core gui

TARGET = SimpleSingleFrontendWithTwoBackendsExample
TEMPLATE = app


SOURCES += main.cpp\
    simplesinglefrontendwidget.cpp \
    simplesinglebackend1.cpp \
    simplesinglebackend2.cpp \
    simplesinglefrontendwithtwobackendsexampletest.cpp

HEADERS  += \
    simplesinglefrontendwidget.h \
    simplesinglebackend1.h \
    simplesinglebackend2.h \
    simplesinglefrontendwithtwobackendsexampletest.h
