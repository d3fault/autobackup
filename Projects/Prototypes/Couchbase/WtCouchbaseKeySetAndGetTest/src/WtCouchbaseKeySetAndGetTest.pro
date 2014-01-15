TARGET = WtCouchbaseKeySetAndGetTest

#QMAKE_CXXFLAGS += -save-temps

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    wtkeysetandgetwidget.cpp

#you can tell how pro an app is by how many -l's it has (then again you could make the same argument saying it's an indication of how shit it is (large projects tend to depend on tons of libraries that support the same feature sets... just becaues different devs 'prefer' specific libraries (or even languages xD)))
LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lboost_thread-mt -lboost_serialization

HEADERS += \
    wtkeysetandgetwidget.h \
    setvaluebykeyrequestfromwt.h
