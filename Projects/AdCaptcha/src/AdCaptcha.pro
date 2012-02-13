#while this is useful to enable me to use Qt Creator to develop Wt apps (auto-complete, simple play button to start etc), the final deployment (freebsd) should not depend on Qt (unless I actually USE Qt, which I don't intend on doing in this project)

QT += core
QT -= gui
TARGET = AdCaptcha
LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lwtdbo -lwtdbosqlite3
#QMAKE_CXXFLAGS += -DNDEBUG
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    site/adcaptchasite.cpp \
    embed/adcaptchaembed.cpp \
    database.cpp \
    site/owners/campaigneditorview.cpp \
    site/publishers/adeditorview.cpp \
    user.cpp

HEADERS += \
    site/adcaptchasite.h \
    embed/adcaptchaembed.h \
    database.h \
    site/owners/campaigneditorview.h \
    site/publishers/adeditorview.h \
    user.h












