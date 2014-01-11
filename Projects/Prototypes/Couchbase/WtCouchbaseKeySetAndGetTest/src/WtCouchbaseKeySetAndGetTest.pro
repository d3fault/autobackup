TARGET = WtCouchbaseKeySetAndGetTest

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    wtkeysetandgetwidget.cpp

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lboost_thread-mt

HEADERS += \
    wtkeysetandgetwidget.h
