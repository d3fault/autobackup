QT += core
QT -= gui
TARGET = procdbanduserdbinterop
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3 -lboost_thread-mt
#QMAKE_CXXFLAGS += -DNDEBUG
QMAKE_CXXFLAGS += -DWT_NO_SLOT_MACROS
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \ 
    wtusersession.cpp \
    AppDbHelper.cpp
HEADERS += \
    wtusersession.h \
    AppDbHelper.h








