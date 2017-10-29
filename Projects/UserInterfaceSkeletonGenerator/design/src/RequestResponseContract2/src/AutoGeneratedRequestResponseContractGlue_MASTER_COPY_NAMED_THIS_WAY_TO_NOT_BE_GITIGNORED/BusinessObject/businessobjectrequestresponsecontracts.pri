INCLUDEPATH *= $$system(pwd)

HEADERS *=	$$system(pwd)/businessobjectrequestresponsecontracts.h \
                $$system(pwd)/someslotrequestresponse.h \
                $$system(pwd)/someslotscopedresponder.h

SOURCES *=	$$system(pwd)/businessobjectrequestresponsecontracts.cpp \
                $$system(pwd)/someslotrequestresponse.cpp \
                $$system(pwd)/someslotscopedresponder.cpp
