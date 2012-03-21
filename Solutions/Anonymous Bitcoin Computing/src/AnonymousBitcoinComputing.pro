QT += core
QT -= gui
TARGET = AdCaptcha
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3
#QMAKE_CXXFLAGS += -DNDEBUG
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    database.cpp \
    user.cpp

HEADERS += \
    anonymousbitcoincomputing.h \
    database.h \
    user.h





