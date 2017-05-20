INCLUDEPATH *= $$system(pwd)/lib

HEADERS *= $$system(pwd)/lib/wasdf.h \
    $$system(pwd)/lib/wasdfarduino.h \
    $$system(pwd)/lib/wasdfcalibrator.h
SOURCES *= $$system(pwd)/lib/wasdf.cpp \
    $$system(pwd)/lib/wasdfarduino.cpp \
    $$system(pwd)/lib/wasdfcalibrator.cpp
