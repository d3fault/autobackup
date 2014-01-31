#This time KISS. I want the last two years of my 20s back :-/... holy shit I am (this.timeLeft() == indefinite ? "smart" : "stupid")...
TARGET = AnonymousBitcoinComputing
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -save-temps

# TESTING VARIABLES (valgrind is slooooooow)
DEFINES +=  NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES=3 \
            WT_TO_COUCHBASE_ADD_MAX_MESSAGES_IN_QUEUE=3 \
            WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE=2400 \
            NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES=3 \
            WT_TO_COUCHBASE_GET_MAX_MESSAGES_IN_QUEUE=3 \
            WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE=500 \
            COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=0 \
            COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT=1 \ #just to give durability_poll something to do on my 1-node cluster test environment
            WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME=\\\"AbcWtToCouchbaseMessageQueuesBaseName\\\" #kill me

# DEPLOY VARIABLES
#CORE TUNABLES (message queue count, message queue max messages, message queue max message size)
#DEFINES +=  NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES=25 \
#            WT_TO_COUCHBASE_ADD_MAX_MESSAGES_IN_QUEUE=200 \
#            WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE=102400 \
#            NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES=125 \
#            WT_TO_COUCHBASE_GET_MAX_MESSAGES_IN_QUEUE=1000 \
#            WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE=500 \ #had '251' to reflect max key size, but that doesn't account for the rest of the message wt->couchbase message. When I'm done hacking shit in, I should do 251+overhead
#            COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=2 \
#            WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME=\\\"AbcWtToCouchbaseMessageQueuesBaseName\\\" #kill me

SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    frontend/anonymousbitcoincomputingwtgui.cpp \
    backend/anonymousbitcoincomputingcouchbasedb.cpp \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.cpp \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.cpp

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lboost_thread-mt -lboost_serialization

HEADERS += \
    anonymousbitcoincomputing.h \
    frontend/anonymousbitcoincomputingwtgui.h \
    backend/anonymousbitcoincomputingcouchbasedb.h \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.h \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.h \
    backend/getandsubscribecacheitem.h