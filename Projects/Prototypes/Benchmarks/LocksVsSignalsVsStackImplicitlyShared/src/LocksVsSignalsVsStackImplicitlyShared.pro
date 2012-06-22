#-------------------------------------------------
#
# Project created by QtCreator 2012-06-22T06:33:57
#
#-------------------------------------------------

QT       += core gui

TARGET = LocksVsSignalsVsStackImplicitlyShared
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    testdriver.cpp \
    Tests/signalcommunicatingheaprecyclingdatagenerator.cpp \
    Tests/signalcommunicatingheaprecyclingdataprocessor.cpp \
    Tests/mutexusingheaprecyclingdatagenerator.cpp \
    Tests/mutexusingheaprecyclingdataprocessor.cpp \
    Tests/signalcommunicatingimplicitlysharedstackdatagenerator.cpp \
    Tests/signalcommunicatingimplicitlysharedstackdataprocessor.cpp

HEADERS  += mainwidget.h \
    testdriver.h \
    Tests/signalcommunicatingheaprecyclingdatagenerator.h \
    Tests/signalcommunicatingheaprecyclingdataprocessor.h \
    Tests/mutexusingheaprecyclingdatagenerator.h \
    Tests/mutexusingheaprecyclingdataprocessor.h \
    Tests/signalcommunicatingimplicitlysharedstackdatagenerator.h \
    Tests/signalcommunicatingimplicitlysharedstackdataprocessor.h


















