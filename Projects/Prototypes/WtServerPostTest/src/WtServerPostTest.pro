QT += core
QT -= gui
TARGET = WtServerPostTest
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3
#QMAKE_CXXFLAGS += -DNDEBUG
QMAKE_CXXFLAGS += -DWT_NO_SLOT_MACROS
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    wtapp.cpp

HEADERS += \
    wtapp.h


