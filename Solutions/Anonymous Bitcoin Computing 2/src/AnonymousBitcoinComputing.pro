#This time KISS. I want the last two years of my 20s back :-/... holy shit I am (this.timeLeft() == indefinite ? "smart" : "stupid")...
#Alpha 0.1, only 332 req To Do items xD. 259 now after one ctrl+f pass, most of them seem dumb. 104 now after 2nd ctrl+f pass and let's call it Beta 0.5 <3

###############DEPLOY############
#1) Copy /usr/share/Wt/resources/ to the directory that "wtAppHere" is located in
#2) In /etc/wt/wt_config.xml, set:
#   2a) <ajax-puzzle>true</ajax-puzzle>
#   2b) <max-request-size>2048</max-request-size> .... 2mb corresponding to "Store Large" defined below (but without message queue header)
#3) Try uncommenting the WStackedWidget's animation code, because I think it needed a css file that (1)'s resource hack thing brings in
#4) Uncomment the 'triggered' connection in ActualLazyLoadedTabWidget (too lazy to update my devbox (whereas my test vm has it xD))
#4) Uncomment DEPLOY VARIABLES below [and comment-out or delete TESTING VARIABLES]
#5) Launch with ./wtAppHere --docroot ".;/usr/local/share/Wt" --http-address 0.0.0.0 --http-port 7777
#                                        ^or just /usr/share/Wt if using Wt from debian repository

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
                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Store=2 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store=2048 \
            \ #set 1 -- Store Large
                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1=StoreLarge \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1=2 \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_StoreLarge=2 \
                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_StoreLarge=1 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_StoreLarge=2157392 \
            \ #set 2 -- Get
                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2=Get \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2=3 \
                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Get=3 \
                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Get=3 \
                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Get=500
DEFINES +=  ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=0 \
            ABC_COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT=1 \ #just to give durability_poll something to do on my 1-node cluster test environment
            ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE=4 \
            ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE=3 \
            ABC_NUM_BITCOINKEYSETS=2

# DEPLOY VARIABLES
#CORE TUNABLES (message queue count, message queue max messages, message queue max message size)
#DEFINES +=  ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS=3 \
#            \ #set 0 -- Store
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET0=Store \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET0=25 \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store=25 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Store=5 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store=10240 \ #10kb per store is probably plenty...
#            \ #set 1 -- Store Large
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1=StoreLarge \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1=6 \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_StoreLarge=6 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_StoreLarge=2 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_StoreLarge=2157392 \ #~2.5mb, to account for overhead + b64encoding
#            \ #set 2 -- Get
#                ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2=Get \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2=125 \
#                ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Get=125 \
#                ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Get=50 \
#                ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Get=500 #had '251' to reflect max key size, but that doesn't account for the rest of the message wt->couchbase message. When I'm done hacking shit in, I should do 251+overhead. problem is i dunno how much space boost serialization is using :-P
#DEFINES +=  ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT=2 \
#            ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE=100 \
#            ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE=100 \ #also is number of bitcoin keys in each range on a hugeBitcoinList page
#            ABC_NUM_BITCOINKEYSETS=1000

# MIN MEMORY REQUIRED [for message queues only] calculated via above tunables: QUEUE_MAX_MESSAGE_SIZE * MAX_MESSAGES_IN_QUEUE * NUMBER_OF_QUEUES

DEFINES +=  ABC_500_INTERNAL_SERVER_ERROR_MESSAGE=\\\"500.Internal.Server.Error\\\" \ #lol can't have spaces what the fuck
            ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME=\\\"AbcWtToCouchbaseMessageQueuesBaseName\\\" \
            ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS=640 \
            ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS=100

HEADERS += \
    anonymousbitcoincomputing.h \
    frontend/anonymousbitcoincomputingwtgui.h \
    backend/anonymousbitcoincomputingcouchbasedb.h \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.h \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.h \
    backend/getandsubscribecacheitem.h \
    frontend/accounttabs/filedeletingfileresource.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    frontend/registersuccessfulwidget.h \
    frontend/validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h \
    frontend/validatorsandinputfilters/safetextvalidatorandinputfilter.h \
    frontend/accounttabs/addfundsaccounttabbody.h \
    frontend/accounttabs/newadslotfilleraccounttabbody.h \
    frontend/accounttabs/viewallexistingadslotfillersaccounttabbody.h \
    frontend/accounttabs/iaccounttabwidgettabbody.h \
    frontend/accounttabs/actuallazyloadedtabwidget.h \
    frontend/accounttabs/singleuseselfdeletingmemoryresource.h \
    frontend/accounttabs/stupidmimefromextensionutil.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/iautoretryingwithexponentialbackoffcouchbaserequest.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasegetrequest.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.h

SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    frontend/anonymousbitcoincomputingwtgui.cpp \
    backend/anonymousbitcoincomputingcouchbasedb.cpp \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.cpp \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.cpp \
    frontend/accounttabs/filedeletingfileresource.cpp \
    frontend/registersuccessfulwidget.cpp \
    frontend/validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.cpp \
    frontend/validatorsandinputfilters/safetextvalidatorandinputfilter.cpp \
    frontend/accounttabs/addfundsaccounttabbody.cpp \
    frontend/accounttabs/newadslotfilleraccounttabbody.cpp \
    frontend/accounttabs/viewallexistingadslotfillersaccounttabbody.cpp \
    frontend/accounttabs/actuallazyloadedtabwidget.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    frontend/accounttabs/singleuseselfdeletingmemoryresource.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/iautoretryingwithexponentialbackoffcouchbaserequest.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasegetrequest.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.cpp

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals-mt -lboost_system-mt -lboost_thread-mt -lboost_serialization
