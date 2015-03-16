#include "anonymousbitcoincomputing.h"

#include <boost/preprocessor/repeat.hpp>
#include <boost/filesystem.hpp>

#include <Wt/WFileResource>

#include <QCoreApplication>
#include <QStringList>
#include <QThread>
#include <QScopedPointer>

#include "abc2common.h"
#include "nonexpiringstringwresource.h"
#include "frontend/anonymousbitcoincomputingwtgui.h"
#include "frontend/gettodaysadslotserver.h"
#include "backend/anonymousbitcoincomputingcouchbasedb.h"

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_GIVE_WT_THE_LOCKFREE_QUEUE_POINTERS(z, n, text) \
    AnonymousBitcoinComputingWtGUI::m_##text##WtMessageQueues[n] = couchbaseDb.get##text##LockFreeQueueForWt##n();
#else
#define ABC_GIVE_WT_THE_LOCKFREE_QUEUE_POINTERS(z, n, text)
#endif

#define ABC_TELL_WT_ABOUT_THE_LIBEVENTS_WE_SET_UP_FOR_IT_TO_SEND_MESSAGES_TO_COUCHBASE_MACRO(z, n, text) \
AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[n] = couchbaseDb.get##text##EventCallbackForWt##n();

/////////////////////////////////////////////////////END MACRO HELL/////////////////////////////////////////////////

#define AnonymousBitcoinComputing_WT_AND_API_SSL_CERT_CLI_ARG "--ssl-certificate="
#define AnonymousBitcoinComputing_WT_AND_API_SSL_PRIVKEY_CLI_ARG "--ssl-private-key="
#define AnonymousBitcoinComputing_API_PORT_CLI_ARG "--api-port"
#define AnonymousBitcoinComputing_OPTIONA_LOCALHOST_ONLY_HTTP_SERVER_FOR_TOR_HIDDEN_SERVICE_PORT "--tor-hidden-service-localhost-only-http-server-api-port" //TODOoptional: describe in --help

int AnonymousBitcoinComputing::startAbcAndWaitForFinished(int argc, char **argv)
{
    (void)argc;
    QStringList argz = QCoreApplication::arguments();
    int indexOfApiPortArg = argz.indexOf(AnonymousBitcoinComputing_API_PORT_CLI_ARG);
    if(indexOfApiPortArg < 0 || ((indexOfApiPortArg+1) >= argz.size()))
    {
        cerr << "You must specify an API port via: " AnonymousBitcoinComputing_API_PORT_CLI_ARG << endl;
        return 1;
    }
    bool parseSuccess = false;
    QString apiPortQString = argz.at(indexOfApiPortArg+1);
    quint16 apiPort = apiPortQString.toUShort(&parseSuccess);
    if(!parseSuccess)
    {
        cerr << "Invalid API port" << endl;
        return 1;
    }
    argz.removeAt(indexOfApiPortArg);
    argz.removeAt(indexOfApiPortArg);

    quint16 torHiddenServiceHttpServerApiPort_OrZeroIfNoneProvided = 0;
    int indexOfTorHiddenServiceHttpServerPortArg = argz.indexOf(AnonymousBitcoinComputing_OPTIONA_LOCALHOST_ONLY_HTTP_SERVER_FOR_TOR_HIDDEN_SERVICE_PORT);
    if((indexOfTorHiddenServiceHttpServerPortArg > -1) && ((indexOfTorHiddenServiceHttpServerPortArg+1) < argz.size()))
    {
        torHiddenServiceHttpServerApiPort_OrZeroIfNoneProvided = argz.at(indexOfTorHiddenServiceHttpServerPortArg+1).toUShort(&parseSuccess);
        if(!parseSuccess)
        {
            cerr << "Invalid Tor Hidden Service HTTP Server API Port" << endl;
            return 1;
        }
        argz.removeAt(indexOfTorHiddenServiceHttpServerPortArg);
        argz.removeAt(indexOfTorHiddenServiceHttpServerPortArg);
    }

    //--ssl-certificate=
    int indexOfSslCertArg = indexOfListItemThatStartsWith(argz, AnonymousBitcoinComputing_WT_AND_API_SSL_CERT_CLI_ARG);
    if(indexOfSslCertArg < 0)
    {
        cerr << "You must provide an " AnonymousBitcoinComputing_WT_AND_API_SSL_CERT_CLI_ARG << endl; //TODOoptional: https OPTIONAL (either compile time or runtime switch)
        return 1;
    }
    QString sslCertArg = argz.at(indexOfSslCertArg);
    sslCertArg.remove(0, QString(AnonymousBitcoinComputing_WT_AND_API_SSL_CERT_CLI_ARG).length());

    //--ssl-private-key=
    int indexOfSslPrivkeyArg = indexOfListItemThatStartsWith(argz, AnonymousBitcoinComputing_WT_AND_API_SSL_PRIVKEY_CLI_ARG);
    if(indexOfSslPrivkeyArg < 0)
    {
        cerr << "You must provide an " AnonymousBitcoinComputing_WT_AND_API_SSL_PRIVKEY_CLI_ARG << endl;
        return 1;
    }
    QString sslPrivkeyArg = argz.at(indexOfSslPrivkeyArg);
    sslPrivkeyArg.remove(0, QString(AnonymousBitcoinComputing_WT_AND_API_SSL_PRIVKEY_CLI_ARG).length());


    //rebuild argv/argc, because Wt is retarded and doesn't ignore extra args -_-
    int newArgC = argz.size();
    char *newArgV[newArgC];
    //this won't work because wt doesn't want an array of scoped pointers :( -- QScopedPointer<char> newArgVs[newArgC];
    int i = 0;
    Q_FOREACH(const QString &currentArg, argz)
    {
        QByteArray currentArgLocal8Bit = currentArg.toLocal8Bit();
        int currentArgLocal8BitSize = currentArgLocal8Bit.size();
        newArgV[i] = new char[currentArgLocal8BitSize];
        //newArgVs[i].reset(new char[currentArgLocal8BitSize]);
        strcpy(newArgV[i], currentArgLocal8Bit.data());
        //strcpy(newArgVs[i].data(), currentArgLocal8Bit.data());
        ++i;
    }
    struct NewArgVscopedDeleter
    {
        int TheArgC;
        char **TheArgV;
        NewArgVscopedDeleter(int theArgC, char **theArgV)
            : TheArgC(theArgC)
            , TheArgV(theArgV)
        { }
        ~NewArgVscopedDeleter()
        {
            for(int j = 0; j < TheArgC; ++j) //TO DOneoptional(that was fun <- nope. written when i had the qscopedpointer impl. this struct shit is dumb): scoped deleter, because we'll leak the memory any time we 'return' above. honestly i can't figure out how to do it xD
            {
                delete [] TheArgV[j];
            }
        }
    };
    NewArgVscopedDeleter newArgVscopedDeleter(newArgC, newArgV);
    (void)newArgVscopedDeleter;


    //start couchbase and wait for it to finish connecting/initializing
    AnonymousBitcoinComputingCouchbaseDB couchbaseDb;
    //TODOoptional: the AnonymousBitcoinComputingCouchbaseDB object should be both instantiated and destructed ON the couchbase thread. However I'm not experiencing any bugs here so fuck it for now

    boost::unique_lock<boost::mutex> couchbaseConnectingLock(*couchbaseDb.isConnectedMutex());
    while(!couchbaseDb.isDoneInitializing())
    {
        couchbaseDb.isConnectedWaitCondition()->wait(couchbaseConnectingLock);
    }
    if(!couchbaseDb.isConnected())
    {
        beginStoppingCouchbase(&couchbaseDb);
        finalStopCouchbaseAndWaitForItsThreadToJoin(&couchbaseDb);
        cout << "libcouchbase finished initializing, but is not connected. Exitting" << endl;
        return 1;
    }

    //couchbase is done initializing/connecting, so now we set up Wt and then start the Wt server

    const std::string &apiPortStdString = apiPortQString.toStdString();
    AnonymousBitcoinComputingWtGUI::setApiHttpsPort(apiPortStdString);

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

    //give wt the lockfree queue pointers. this is pretty much equivalent to newAndOpenAllWtMessageQueues, except for lockfree queue
    //AnonymousBitcoinComputingWtGUI::m_StoreWtMessageQueues[n] = couchbaseDb.getStoreLockFreeQueueForWt0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_GIVE_WT_THE_LOCKFREE_QUEUE_POINTERS)

    //register the wt -> couchbase events
    //AnonymousBitcoinComputingWtGUI::m_StoreEventCallbacksForWt[0] = couchbaseDb.getStoreEventCallbackForWt0();
    BOOST_PP_REPEAT(ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS, ABC_WT_TO_COUCHBASE_MESSAGE_QUEUES_FOREACH_SET_BOOST_PP_REPEAT_AGAIN_MACRO, ABC_TELL_WT_ABOUT_THE_LIBEVENTS_WE_SET_UP_FOR_IT_TO_SEND_MESSAGES_TO_COUCHBASE_MACRO)

    //start server, waitForShutdown(), event_active a couchbase event to let it finish current actions (also sets bool to not allow further), server.stop, tell couchbase to join, wait for couchbase to join
    WServer wtServer(argv[0]);

    wtServer.setServerConfiguration(newArgC, newArgV, WTHTTP_CONFIGURATION);

    //BEGIN ABC LOGO
    streampos fileSizeHack;
    char *abcLogoBuffer;
    ifstream abcLogoFileStream("abc.logo.svg", ios::in | ios::binary | ios::ate);
    if(abcLogoFileStream.is_open())
    {
        fileSizeHack = abcLogoFileStream.tellg();
        abcLogoFileStream.seekg(0,ios::beg);
        abcLogoBuffer = new char[fileSizeHack]; //TODOoptimization: running out of memory (server critical load) throws an exception, we could pre-allocate this, but how to then share it among WApplications? guh fuckit. i think 'new' is mutex protected underneath anyways, so bleh maybe using a mutex and a pre-allocated buffer is even faster than this (would of course be best to use rand() + mutex shits xD)... but then we can't have multiple doing it on thread pool at same time (unless rand() + mutex shits)
        abcLogoFileStream.read(abcLogoBuffer, fileSizeHack);
        abcLogoFileStream.close();
    }
    else
    {
        beginStoppingCouchbase(&couchbaseDb);
        finalStopCouchbaseAndWaitForItsThreadToJoin(&couchbaseDb);
        cerr << "failed to open abc.logo.svg, quitting" << endl;
        return 1;
    }
    std::string abcLogoString = std::string(abcLogoBuffer, fileSizeHack);
    delete [] abcLogoBuffer;
    NonExpiringStringWResource abcLogoImageResource(abcLogoString, "image/svg+xml", WResource::Inline, "abc.logo.svg");
    abcLogoString.clear(); //we made a copy already. TODOoptimization similar calls can go in hvbs i suppose
    wtServer.addResource(&abcLogoImageResource, "/abc.logo.svg");
    //END ABC LOGO

    //BEGIN SIMPLE PHP API CLIENT
    if(!boost::filesystem::exists(ABC_API_SIMPLE_PHP_CLIENT_FILENAME))
    {
        beginStoppingCouchbase(&couchbaseDb);
        finalStopCouchbaseAndWaitForItsThreadToJoin(&couchbaseDb);
        cerr << ABC_API_SIMPLE_PHP_CLIENT_FILENAME " does not exist. quitting" << endl;
        return 1;
    }
    WFileResource simplePhpApiClientFileResource(ABC_API_SIMPLE_PHP_CLIENT_MIME_TYPE, ABC_API_SIMPLE_PHP_CLIENT_FILENAME);
    simplePhpApiClientFileResource.suggestFileName(ABC_API_SIMPLE_PHP_CLIENT_FILENAME);
    wtServer.addResource(&simplePhpApiClientFileResource, ABC_API_SIMPLE_PHP_CLIENT_URL);
    //END SIMPLE PHP API CLIENT

    //GetTodaysAdSlotResource getTodaysAdSlotResource;
    //wtServer.addResource(&getTodaysAdSlotResource, "/todays-ad.json");

    wtServer.addEntryPoint(Application, &AnonymousBitcoinComputing::createAnonymousBitcoinComputingWtGUI);

    //BEGIN GetTodaysAdSlotServer
    GetTodaysAdSlotServer::setBackendGetQueue(couchbaseDb.getGetApiLockFreeQueue());
    GetTodaysAdSlotServer::setBackendStoreQueue(couchbaseDb.getStoreApiLockFreeQueue());
    GetTodaysAdSlotServer::setBackendGetQueueEvent(couchbaseDb.getGetApiEventCallbackForWt());
    GetTodaysAdSlotServer::setBackendStoreQueueEvent(couchbaseDb.getStoreApiEventCallbackForWt());
    QScopedPointer<GetTodaysAdSlotServer, QScopedPointerDeleteLater> getTodaysAdSlotServer(new GetTodaysAdSlotServer());
    QThread getTodaysAdSlotServerThread;
    getTodaysAdSlotServerThread.start();
    getTodaysAdSlotServer->moveToThread(&getTodaysAdSlotServerThread);
    bool getTodaysAdSlotServerInitializedAndStartedSuccessfully = false;
    QMetaObject::invokeMethod(getTodaysAdSlotServer.data(), "initializeAndStart", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, getTodaysAdSlotServerInitializedAndStartedSuccessfully), Q_ARG(quint16, apiPort), Q_ARG(QString, sslCertArg), Q_ARG(QString, sslPrivkeyArg), Q_ARG(quint16, torHiddenServiceHttpServerApiPort_OrZeroIfNoneProvided));
    if(!getTodaysAdSlotServerInitializedAndStartedSuccessfully)
    {
        beginStoppingCouchbase(&couchbaseDb);
        getTodaysAdSlotServer.reset();
        getTodaysAdSlotServerThread.quit();
        getTodaysAdSlotServerThread.wait();
        finalStopCouchbaseAndWaitForItsThreadToJoin(&couchbaseDb);
        cerr << "failed to initialize and start GetTodaysAdSlotServer, quitting" << endl;
        return 1;
    }
    //END GetTodaysAdSlotServer


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

    //BEGIN GetTodaysAdSlotServer
    QMetaObject::invokeMethod(getTodaysAdSlotServer.data(), "stopAndDestroy", Qt::BlockingQueuedConnection);
    getTodaysAdSlotServer.reset();
    getTodaysAdSlotServerThread.quit();
    getTodaysAdSlotServerThread.wait();
    //END //BEGIN GetTodaysAdSlotServer

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
WApplication *AnonymousBitcoinComputing::createAnonymousBitcoinComputingWtGUI(const WEnvironment &env)
{
    return new AnonymousBitcoinComputingWtGUI(env);
}
void AnonymousBitcoinComputing::beginStoppingCouchbase(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb)
{
    boost::unique_lock<boost::mutex> couchbaseHasFinishedAllPendingRequestsLock(*couchbaseDb->isFinishedWithAllPendingRequestsMutex());

    //tell couchbase to finish current and not accept more
    if(couchbaseDb->isInEventLoop())
    {
        event_active(couchbaseDb->beginStoppingCouchbaseCleanlyEvent(), EV_READ|EV_WRITE, 0);
    }

    //TODOoptional: this is where we could event_active an event that creates a libevent timeout to murder couchbase if it doesn't finish before the timeout

    //wait until couchbase has finished current
    while(!couchbaseDb->isFinishedWithAllPendingRequests())
    {
        couchbaseDb->isFinishedWithAllPendingRequestsWaitCondition()->wait(couchbaseHasFinishedAllPendingRequestsLock);
    }
}
void AnonymousBitcoinComputing::finalStopCouchbaseAndWaitForItsThreadToJoin(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb)
{
    event_active(couchbaseDb->finalCleanUpAndJoinEvent(), EV_READ|EV_WRITE, 0);
    couchbaseDb->waitForJoin();
}
