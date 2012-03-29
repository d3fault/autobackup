QT += core
QT -= gui
TARGET = QtBroadcastServerWtClient
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3 -lboost_thread-mt
#QMAKE_CXXFLAGS += -DNDEBUG
#QMAKE_CXXFLAGS += -DWT_NO_SLOT_MACROS
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app 

SOURCES += main.cpp \
    qtbroadcastservernumberwatcher.cpp \
    wtnumberwatchingwidget.cpp \
    myrandomnumbergenerator.cpp

HEADERS += \
    qtbroadcastservernumberwatcher.h \
    wtnumberwatchingwidget.h \
    myrandomnumbergenerator.h





