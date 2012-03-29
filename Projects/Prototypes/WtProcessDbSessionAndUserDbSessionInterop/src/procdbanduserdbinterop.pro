QT += core
QT -= gui
TARGET = procdbanduserdbinterop
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3 -lboost_thread-mt
#QMAKE_CXXFLAGS += -DNDEBUG
QMAKE_CXXFLAGS += -DWT_NO_SLOT_MACROS
CONFIG += console
#CONFIG += no_keywords
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \ 
    wtusersession.cpp \
    ../../../../tps/libqtwtcompat/WQApplication.C \
    ../../../../tps/libqtwtcompat/DispatchThread.C

HEADERS += \
    wtusersession.h \
    ../../../../tps/libqtwtcompat/WQApplication \
    ../../../../tps/libqtwtcompat/DispatchThread.h


