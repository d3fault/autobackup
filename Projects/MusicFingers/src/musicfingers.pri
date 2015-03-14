INCLUDEPATH += $$system(pwd)/lib
HEADERS +=  $$system(pwd)/lib/musicfingers.h \
    $$PWD/lib/musicfingerssynthesizer.h \
    $$PWD/lib/finger.h
SOURCES +=  $$system(pwd)/lib/musicfingers.cpp \
    $$PWD/lib/musicfingerssynthesizer.cpp \
    $$PWD/lib/finger.cpp
