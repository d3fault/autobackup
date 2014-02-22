#This time KISS. I want the last two years of my 20s back :-/... holy shit I am (this.timeLeft() == indefinite ? "smart" : "stupid")...
#Alpha 0.1, only 332 req To Do items xD. 259 now after one ctrl+f pass, most of them seem dumb

###############DEPLOY############
#1) Copy /usr/share/Wt/resources/ to the directory that "wtAppHere" is located in
#2) In /etc/wt/wt_config.xml, set:
#   2a) <ajax-puzzle>true</ajax-puzzle>
#   2b) <max-request-size>2048</max-request-size> .... 2mb corresponding to "Store Large" defined below (but without message queue header)
#3) Try uncommenting the WStackedWidget's animation code, because I think it needed a css file that (1)'s resource hack thing brings in
#4) Uncomment DEPLOY VARIABLES below [and comment-out or delete TESTING VARIABLES]
#5) Launch with ./wtAppHere --docroot ".;./resources/" --http-address 0.0.0.0 --http-port 7777

TARGET = AnonymousBitcoinComputing
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -save-temps

# TESTING VARIABLES (valgrind is slooooooow)
DEFINES +=  ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS=3 \
            \ #set 0 -- Store
                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET0=Store \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET0=5 \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store=5 \
                MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Store=2 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store=2048 \
            \ #set 1 -- Store Large
                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1=StoreLarge \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1=2 \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_StoreLarge=2 \
                MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_StoreLarge=1 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_StoreLarge=2097152 \
            \ #set 2 -- Get
                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2=Get \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2=3 \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Get=3 \
                MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Get=3 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Get=500
DEFINES +=  ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=0 \
            ABC_COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT=1 \ #just to give durability_poll something to do on my 1-node cluster test environment
            ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME=\\\"AbcWtToCouchbaseMessageQueuesBaseName\\\" \
            ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE=4 \
            ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE=3 \
            ABC_NUM_BITCOINKEYSETS=2

# DEPLOY VARIABLES
#CORE TUNABLES (message queue count, message queue max messages, message queue max message size)
#DEFINES +=  ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS=3 \
#            \ #set 0 -- Store
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET0=Store \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET0=25 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_SET0=5 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_SET0=10240 \ #10kb per store is probably plenty... TODOreq: code needs to check sizes before trying, or sanitize all user inputs in some other way
#            \ #set 1 -- Store Large
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1=StoreLarge \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1=3 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_SET1=2 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_SET1=2097152 \ #2mb + 10kb overhead
#            \ #set 2 -- Get
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2=Get \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2=125 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_SET2=50 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_SET2=500 #had '251' to reflect max key size, but that doesn't account for the rest of the message wt->couchbase message. When I'm done hacking shit in, I should do 251+overhead. problem is i dunno how much space boost serialization is using :-P
#DEFINES +=  ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=2 \
#            ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME=\\\"AbcWtToCouchbaseMessageQueuesBaseName\\\"
#            ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE=100 \
#            ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE=100 \ #also is number of bitcoin keys in each range on a hugeBitcoinList page
#            ABC_NUM_BITCOINKEYSETS=1000

# MIN MEMORY REQUIRED [for message queues only] calculated via above tunables: QUEUE_MAX_MESSAGE_SIZE * MAX_MESSAGES_IN_QUEUE * NUMBER_OF_QUEUES

DEFINES +=  ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS=640 \
            ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS=100

HEADERS += \
    anonymousbitcoincomputing.h \
    frontend/anonymousbitcoincomputingwtgui.h \
    backend/anonymousbitcoincomputingcouchbasedb.h \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.h \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.h \
    backend/getandsubscribecacheitem.h \
    frontend/filedeletingfileresource.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h

SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    frontend/anonymousbitcoincomputingwtgui.cpp \
    backend/anonymousbitcoincomputingcouchbasedb.cpp \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.cpp \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.cpp \
    frontend/filedeletingfileresource.cpp

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lboost_thread-mt -lboost_serialization
