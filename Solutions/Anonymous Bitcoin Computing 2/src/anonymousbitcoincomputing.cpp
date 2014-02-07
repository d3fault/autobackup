#include "anonymousbitcoincomputing.h"

#include "frontend/anonymousbitcoincomputingwtgui.h"
#include "backend/anonymousbitcoincomputingcouchbasedb.h"

int AnonymousBitcoinComputing::startAbcAndWaitForFinished(int argc, char **argv)
{
    //start couchbase and wait for it to finish connecting/initializing
    struct AnonymousBitcoinComputingCouchbaseDBScopedDeleter
    {
        //TODOreq: the AnonymousBitcoinComputingCouchbaseDB object should be both instantiated and destructed ON the couchbase thread. However I'm not experiencing any bugs here so fuck it for now
        AnonymousBitcoinComputingCouchbaseDB *CouchbaseDb;
        AnonymousBitcoinComputingCouchbaseDBScopedDeleter()
            : CouchbaseDb(new AnonymousBitcoinComputingCouchbaseDB())
        { }
        ~AnonymousBitcoinComputingCouchbaseDBScopedDeleter()
        {
            delete CouchbaseDb;
        }
    } AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance;
    boost::unique_lock<boost::mutex> couchbaseConnectingLock(*AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isConnectedMutex());
    while(!AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isDoneInitializing())
    {
        AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isConnectedWaitCondition()->wait(couchbaseConnectingLock);
    }
    if(!AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isConnected())
    {
        //TODOreq: maybe AnonymousBitcoinComputingCouchbaseDB's destructor does event_break and joins the thread etc? All I know is I need more shit here (but really it's a corner case and unlikely [to go unnoticed] so fuck it for now)
        return 1;
    }

    //couchbase is done initializing/connecting, so now we set up Wt and then start the Wt server

    struct TellWtToNewAndOpenButEventuallyCloseAndDeleteMessageQueuesWeKnowItHasScopedDeleter
    {
        TellWtToNewAndOpenButEventuallyCloseAndDeleteMessageQueuesWeKnowItHasScopedDeleter()
        {
            AnonymousBitcoinComputingWtGUI::newAndOpenAllWtMessageQueues();
        }
        ~TellWtToNewAndOpenButEventuallyCloseAndDeleteMessageQueuesWeKnowItHasScopedDeleter()
        {
            AnonymousBitcoinComputingWtGUI::deleteAllWtMessageQueues();
        }
    } TellWtToNewAndOpenButEventuallyCloseAndDeleteMessageQueuesWeKnowItHasScopedDeleterInstance;


    //register the wt -> couchbase events
    //AnonymousBitcoinComputingWtGUI::m_StoreEventCallbacksForWt[0] = AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->getStoreEventCallbackForWt0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_TELL_WT_ABOUT_THE_LIBEVENTS_WE_SET_UP_FOR_IT_TO_SEND_MESSAGES_TO_COUCHBASE_MACRO)

    //start server, waitForShutdown(), event_active a couchbase event to let it finish current actions (also sets bool to not allow further), server.stop, tell couchbase to join, wait for couchbase to join
    WServer wtServer(argv[0]);

    wtServer.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    wtServer.addEntryPoint(Application, &AnonymousBitcoinComputing::createAnonymousBitcoinComputingWtGUI);

    int ret = 0;
    if(wtServer.start())
    {
        ret = wtServer.waitForShutdown();
        if(ret == 2 || ret == 3 || ret == 15) //SIGINT, SIGQUIT, SIGTERM, respectively (TODOportability: are these the same on windows?). Any other signal we return verbatim
        {
            ret = 0;
        }
    }
    else
    {
        ret = 1;
        //TODOreq: might need to do more special handling here idfk
    }

    //telling them to FINISH [current requests] != JOIN [and cleanup]. finish sets flags to not do any more operations, and lets current operations finish cleanly and then raises a wait condition when all current are done. It does NOT however delete/free any of the event callbacks, SINCE THE WT SERVER IS STILL RUNNING (and therefore using them (even though when it does use one, it gets instantly rejected because of the bool flags))

    boost::unique_lock<boost::mutex> couchbaseHasFinishedAllPendingRequestsLock(*AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isFinishedWithAllPendingRequestsMutex());

    //tell couchbase to finish current and not accept more
    if(AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isInEventLoop())
    {
        event_active(AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->beginStoppingCouchbaseCleanlyEvent(), EV_READ|EV_WRITE, 0);
    }

    //TODOoptional: this is where we could event_active an event that creates a libevent timeout to murder couchbase if it doesn't finish before the timeout

    //wait until couchbase has finished current
    while(!AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isFinishedWithAllPendingRequests())
    {
        AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->isFinishedWithAllPendingRequestsWaitCondition()->wait(couchbaseHasFinishedAllPendingRequestsLock);
    }

    //then:
    wtServer.stop();

    //tell couchbase to clean up and join (this is where we break the event loop)
    event_active(AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->finalCleanUpAndJoinEvent(), EV_READ|EV_WRITE, 0);

    AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->waitForJoin();

    if(!AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->threadExittedCleanly())
    {
        cout << "Couchbase thread did not exit cleanly" << endl;
        if(ret == 0)
        {
            return 2;
        }
        return ret;
    }
    if(ret == 0)
    {
        cout << "Now Exitting Cleanly" << endl;
    }
    else
    {
        cout << "NOT exitting cleanly, return code: " << ret << endl;
    }
    return ret;
    //cleanup is done in the scoped structs
}
WApplication *AnonymousBitcoinComputing::createAnonymousBitcoinComputingWtGUI(const WEnvironment &env)
{
    return new AnonymousBitcoinComputingWtGUI(env);
}
