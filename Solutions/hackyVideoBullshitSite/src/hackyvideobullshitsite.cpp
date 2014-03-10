#include "hackyvideobullshitsite.h"

#include <boost/thread.hpp>

#include "boostthreadinitializationsynchronizationkit.h"
#include "adimagegetandsubscribemanager.h"

int HackyVideoBullshitSite::startHackyVideoBullshitSiteAndWaitForFinished(int argc, char *argv[])
{
    //start ad image get and subscribe thread and wait for it to finish initializing
    BoostThreadInitializationSynchronizationKit<AdImageGetAndSubscribeManager> threadInitializationSynchronizationKit;
    struct AdImageGetAndSubscribeThreadScopedDeleter
    {
        boost::thread m_AdImageGetAndSubscribeThread;
        AdImageGetAndSubscribeThreadScopedDeleter()
            : m_AdImageGetAndSubscribeThread(boost::bind(boost::bind(&HackyVideoBullshitSite::adImageGetAndSubscribeThreadEntryPoint, this, _1), &threadInitializationSynchronizationKit))
        { }
        ~AdImageGetAndSubscribeThreadScopedDeleter()
        {
            m_AdImageGetAndSubscribeThread.join();
        }
    } AdImageGetAndSubscribeThreadScopedDeleterInstance;

    boost::lock_guard<boost::mutex> adImageThreadFinishedInitializingLock(threadInitializationSynchronizationKit.InitializationMutex);
    while(!threadInitializationSynchronizationKit.isInitialized)
    {
        threadInitializationSynchronizationKit.InitializationWaitCondition.wait(adImageThreadFinishedInitializingLock);
    }

    //AdImageGetAndSubscribeManager is done initializing, so now we set up Wt and then start the Wt server

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
    //AnonymousBitcoinComputingWtGUI::m_StoreEventCallbacksForWt[0] = couchbaseDb.getStoreEventCallbackForWt0();
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
    }

    //telling them to FINISH [current requests] != JOIN [and cleanup]. finish sets flags to not do any more operations, and lets current operations finish cleanly and then raises a wait condition when all current are done. It does NOT however delete/free any of the event callbacks, SINCE THE WT SERVER IS STILL RUNNING (and therefore using them (even though when it does use one, it gets instantly rejected because of the bool flags))
    beginStoppingCouchbase(&couchbaseDb);

    //then:
    wtServer.stop(); //TODOoptional: start may have returned false and it might not be good to call this (bah)

    //tell couchbase to clean up and join (this is where we break the event loop)
    finalStopCouchbaseAndWaitForItsThreadToJoin(&couchbaseDb);

    if(!couchbaseDb.threadExittedCleanly())
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
void HackyVideoBullshitSite::adImageGetAndSubscribeThreadEntryPoint(BoostThreadInitializationSynchronizationKit *threadInitializationSynchronizationKit)
{
    AdImageGetAndSubscribeManager adImageGetAndSubscribeManager;
    adImageGetAndSubscribeManager.startManagingAdImageSubscriptionsAndWaitUntilToldToStop(threadInitializationSynchronizationKit);
}
