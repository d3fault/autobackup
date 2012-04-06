QT += core
QT -= gui
TARGET = AnonymousBitcoinComputing
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3
#QMAKE_CXXFLAGS += -DNDEBUG
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    database.cpp \
    usernamedb.cpp \
    views/abchome.cpp \
    views/abcindex.cpp \
    views/balance/abcbalanceaddfunds.cpp \
    views/balance/abcbalancerequestpayout.cpp \
    views/advertising/abcadvertisingselladspace.cpp \
    views/advertising/abcadvertisingbuyadspace.cpp

HEADERS += \
    anonymousbitcoincomputing.h \
    database.h \
    usernamedb.h \
    views/abchome.h \
    views/abcindex.h \
    views/balance/abcbalanceaddfunds.h \
    views/balance/abcbalancerequestpayout.h \
    views/advertising/abcadvertisingselladspace.h \
    views/advertising/abcadvertisingbuyadspace.h




