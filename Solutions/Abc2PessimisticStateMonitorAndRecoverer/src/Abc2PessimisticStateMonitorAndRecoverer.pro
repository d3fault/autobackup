TARGET = Abc2PessimisticStateMonitorAndRecoverer
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    abc2pessimisticstatemonitorandrecoverer.cpp

HEADERS += \
    abc2pessimisticstatemonitorandrecoverer.h

LIBS += -lcouchbase
