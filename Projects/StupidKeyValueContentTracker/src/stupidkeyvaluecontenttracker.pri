INCLUDEPATH *= $$system(pwd)/lib

HEADERS *=	$$system(pwd)/lib/stupidkeyvaluecontenttracker.h \
		$$system(pwd)/lib/ikeyvaluestoremutation.h \
		$$system(pwd)/lib/keyvaluestoremutation_add.h \
		$$system(pwd)/lib/stupidkeyvaluecontenttrackertypes.h \
		$$system(pwd)/lib/keyvaluestoremutationfactory.h \
		$$system(pwd)/lib/keyvaluestoremutation_remove.h \
		$$system(pwd)/lib/keyvaluestoremutation_modify.h

SOURCES *=	$$system(pwd)/lib/stupidkeyvaluecontenttracker.cpp \
		$$system(pwd)/lib/keyvaluestoremutation_add.cpp \
		$$system(pwd)/lib/keyvaluestoremutationfactory.cpp \
		$$system(pwd)/lib/keyvaluestoremutation_remove.cpp \
		$$system(pwd)/lib/keyvaluestoremutation_modify.cpp

include(../../TimeAndData_Timeline/src/timeanddata_timeline.pri)
