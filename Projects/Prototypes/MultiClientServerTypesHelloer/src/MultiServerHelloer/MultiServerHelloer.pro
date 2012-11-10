QT       += core gui

TARGET = MultiServerHelloer
TEMPLATE = app


SOURCES += main.cpp\
        serverwidget.cpp \
    multiserverhelloertest.cpp \
    multiserverhelloer.cpp \
    multiserverbusiness.cpp

HEADERS  += serverwidget.h \
    ../ServerClientShared/helloprotocol.h \
    multiserverhelloertest.h \
    multiserverhelloer.h \
    multiserverbusiness.h

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
