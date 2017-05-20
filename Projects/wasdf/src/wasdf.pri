INCLUDEPATH *= $$system(pwd)/lib

HEADERS *= $$system(pwd)/lib/wasdf.h \
    lib/wasdfarduino.h \
    lib/wasdfcalibrator.h \
    lib/wasdfcalibrationconfiguration.h
SOURCES *= $$system(pwd)/lib/wasdf.cpp \
    lib/wasdfarduino.cpp \
    lib/wasdfcalibrator.cpp
