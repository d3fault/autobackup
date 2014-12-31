#This time KISS. I want the last two years of my 20s back :-/... holy shit I am (this->timeLeft() == indefinite ? "smart" : "stupid")...
#Alpha 0.1, only 332 req To Do items xD. 259 now after one ctrl+f pass, most of them seem dumb. 104 now after 2nd ctrl+f pass and let's call it Beta 0.5 <3

###############DEPLOY############
#0) Give system a a huge swap. This app does not deal with out of memory scenarios well (or at all ;-P)
#1) Copy /usr/share/Wt/resources/ to the directory that "wtAppHere" is located in
#2) In /etc/wt/wt_config.xml, set:
#   2a) <ajax-puzzle>true</ajax-puzzle>
#   2b) <max-request-size>175</max-request-size> .... 175 kb corresponding to 576x96 24-bit uncompressed BMP ad slot filler, which comes out to 163.x kb max, but I am not sure if that includes the http headers so 175 sounds fine...
#3) Try uncommenting the WStackedWidget's animation code, because I think it needed a css file that (1)'s resource hack thing brings in
#4) Uncomment DEPLOY VARIABLES below [and comment-out or delete TESTING VARIABLES]
#5) Turn on auto-failover
#5) cd into wtAppHere and `ln -s /usr/local/share/Wt/resources/ resources`
#                                   ^or just /usr/share/Wt if using Wt from debian repository
#6a) Launch with ./wtAppHere --docroot ".;/resources" --http-address 0.0.0.0 --http-port 7777
#6b) Launch with ./wtAppHere --docroot ".;/resources" --http-address 0.0.0.0 --http-port 80 --ssl-certificate=server.pem --ssl-private-key=server.key --ssl-tmp-dh=dh512.pem --https-address 0.0.0.0 --https-port 443 --api-port 444

TARGET = AnonymousBitcoinComputing
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
#CONFIG -= qt
QT += core network
QT -= gui

#QMAKE_CXXFLAGS += -save-temps

#intermittent (<3 multi-threaded bugs) segfault without this define!
DEFINES += BOOST_SPIRIT_THREADSAFE

DEFINES += ABC_MULTI_CAMPAIGN_OWNER_MODE

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
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.h \
    ../../../GloballySharedClasses/distributeddatabase/d3faultscouchbaseshared.h \
    ../../../GloballySharedClasses/image/nonanimatedimagheaderchecker.h \
    abc2common.h \
    ../../hackyVideoBullshitSite/src/backend/nonexpiringstringwresource.h \
    frontend/pages/advertisingbuyownersadspacecampaignwithindexwidget.h \
    frontend/pages/advertisingselladspacecreatenewadcampaignwidget.h \
    frontend2backendRequests/viewquerycouchbasedocumentbykeyrequest.h \
    frontend/pages/advertisingbuyadspacealluserswithatleastoneadcampaignwidget.h \
    frontend/pages/advertisingbuyadspacealladcampaignsforuserwidget.h \
    frontend/accounttabs/withdrawfundsaccounttabbody.h \
    frontend/gettodaysadslotresource.h \
    frontend/gettodaysadslotserver.h \
    frontend/gettodaysadslotserverclientconnection.h \
    frontend2backendRequests/igetcouchbasedocumentbykeyrequestresponder.h \
    frontend2backendRequests/abcguigetcouchbasedocumentbykeyrequestresponder.h \
    frontend2backendRequests/abcapigetcouchbasedocumentbykeyrequestresponder.h \
    frontend2backendRequests/abcguistorecouchbasedocumentbykeyrequestresponder.h \
    frontend2backendRequests/istorecouchbasedocumentbykeyrequestresponder.h \
    frontend2backendRequests/abcapistorecouchbasedocumentbykeyrequestresponder.h

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
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.cpp \
    ../../hackyVideoBullshitSite/src/backend/nonexpiringstringwresource.cpp \
    frontend/pages/advertisingbuyownersadspacecampaignwithindexwidget.cpp \
    backend/getandsubscribecacheitem.cpp \
    frontend/pages/advertisingselladspacecreatenewadcampaignwidget.cpp \
    frontend2backendRequests/viewquerycouchbasedocumentbykeyrequest.cpp \
    frontend/pages/advertisingbuyadspacealluserswithatleastoneadcampaignwidget.cpp \
    frontend/pages/advertisingbuyadspacealladcampaignsforuserwidget.cpp \
    frontend/accounttabs/withdrawfundsaccounttabbody.cpp \
    frontend/gettodaysadslotresource.cpp \
    frontend/gettodaysadslotserver.cpp \
    frontend/gettodaysadslotserverclientconnection.cpp \
    frontend2backendRequests/abcguigetcouchbasedocumentbykeyrequestresponder.cpp \
    frontend2backendRequests/abcapigetcouchbasedocumentbykeyrequestresponder.cpp \
    frontend2backendRequests/abcguistorecouchbasedocumentbykeyrequestresponder.cpp \
    frontend2backendRequests/abcapistorecouchbasedocumentbykeyrequestresponder.cpp

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/
INCLUDEPATH += ../../../GloballySharedClasses/image/
INCLUDEPATH += ../../hackyVideoBullshitSite/src/backend/

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals -lboost_system -lboost_thread -lboost_serialization #TODOoptional: serialization lib not necessary when using lockfree queue
