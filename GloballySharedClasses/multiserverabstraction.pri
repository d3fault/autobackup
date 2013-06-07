INCLUDEPATH += $$system(pwd)/network

HEADERS += $$system(pwd)/network/multiserverabstraction.h \
		$$system(pwd)/network/networkmagic.h \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.h \
		$$system(pwd)/network/abstractclientconnection.h \
                $$system(pwd)/network/iprotocolknower.h \
                $$system(pwd)/network/iprotocolknowerfactory.h \
                $$system(pwd)/network/abstractconnection.h

SOURCES += $$system(pwd)/network/multiserverabstraction.cpp \
		$$system(pwd)/network/networkmagic.cpp \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.cpp \
		$$system(pwd)/network/abstractclientconnection.cpp \
                $$system(pwd)/network/abstractconnection.cpp

include($$system(pwd)/ssltcpserver.pri)
