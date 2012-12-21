INCLUDEPATH += $$system(pwd)/network

HEADERS += $$system(pwd)/network/multiserverabstraction.h \
		$$system(pwd)/network/networkmagic.h \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.h

SOURCES += $$system(pwd)/network/multiserverabstraction.cpp \
		$$system(pwd)/network/networkmagic.cpp \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.cpp
