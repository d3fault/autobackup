#include "hackyvideobullshitsite.h"

#include <fstream>

#include <QCoreApplication>
#include <QThread>
#include <QAtomicInt>

#include "backend/hackyvideobullshitsitebackend.h"
#include "backend/adimagewresource.h"
#include "frontend/hackyvideobullshitsitegui.h"

int HackyVideoBullshitSite::startHackyVideoBullshitSiteAndWaitForFinished(int argc, char *argv[])
{
    QCoreApplication qapp(argc, argv); //not sure if this is necessary (I've seen various documentation saying "can't call this until QCoreApplication is instantiated")... but probably won't hurt. I know I don't need to call qapp.exec at least

    //start ad image get and subscribe thread and wait for it to finish initializing
    struct HackyVideoBullshitSiteBackendScopedDeleter
    {
        QAtomicInt m_SharedVideoSegmentsArrayIndex; //main object for VideoSegmentsImporterFolderWatcher -> Wt communication
        HackyVideoBullshitSiteBackend m_HackyVideoBullshitSiteBackend;
        QThread m_HackyVideoBullshitSiteBackendThread;
        HackyVideoBullshitSiteBackendScopedDeleter()
                : m_SharedVideoSegmentsArrayIndex(-1)
                , m_HackyVideoBullshitSiteBackend(&m_SharedVideoSegmentsArrayIndex)
        {
            //using a style i dislike (object not instantiating on thread that 'owns' it, but oh well)
            m_HackyVideoBullshitSiteBackend.moveToThread(&m_HackyVideoBullshitSiteBackendThread);
            m_HackyVideoBullshitSiteBackendThread.start();
        }
        ~HackyVideoBullshitSiteBackendScopedDeleter()
        {
            m_HackyVideoBullshitSiteBackendThread.quit();
            m_HackyVideoBullshitSiteBackendThread.wait();
        }
    } HackyVideoBullshitSiteBackendScopedDeleterInstance;

    WServer wtServer(argv[0]);
    wtServer.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    wtServer.addEntryPoint(Application, &HackyVideoBullshitSite::hackyVideoBullshitSiteGuiEntryPoint);
    //add the no ad global/public resource

    WResource *noAdPlaceholderImageResource = 0;

    {
        streampos fileSizeHack;
        char *noAdPlaceholderImageBuffer;
        ifstream noAdPlaceholderImageFileStream("no.ad.placeholder.jpg", ios::in | ios::binary | ios::ate);
        if(noAdPlaceholderImageFileStream.is_open())
        {
            fileSizeHack = noAdPlaceholderImageFileStream.tellg();
            noAdPlaceholderImageFileStream.seekg(0,ios::beg);
            noAdPlaceholderImageBuffer = new char[fileSizeHack];
            noAdPlaceholderImageFileStream.read(noAdPlaceholderImageBuffer, fileSizeHack);
            noAdPlaceholderImageFileStream.close();
        }
        else
        {
            cerr << "failed to open no ad image placeholder for reading" << endl;
            return 1;
        }
        std::string noAdPlaceholderImageString = std::string(noAdPlaceholderImageBuffer, fileSizeHack);
        noAdPlaceholderImageResource = new AdImageWResource(noAdPlaceholderImageString, "image/jpeg", "image.jpg", WResource::Inline);
        delete [] noAdPlaceholderImageBuffer;
    }

    wtServer.addResource(noAdPlaceholderImageResource, "/no.ad.placeholder.jpg");

    QMetaObject::invokeMethod(&HackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "initializeAndStart", Qt::BlockingQueuedConnection);

    //AdImageGetAndSubscribeManager is done initializing, so now we set up Wt and then start the Wt server

    HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = HackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend.adImageGetAndSubscribeManager();
    HackyVideoBullshitSiteGUI::m_SharedVideoSegmentsArrayIndex = &HackyVideoBullshitSiteBackendScopedDeleterInstance.m_SharedVideoSegmentsArrayIndex; //similarly, instead of a pointer to a video segment file, i could point to a plugin to expand capability without shutting the server down :-P

    //start server, waitForShutdown(), invoke via BlockingQueuedConnection a 'stop' to AdImageGetAndSubscribeManager to let it finish current actions (also sets bool to not allow further), server.stop, tell AdImageGetAndSubscribeManager to quit, wait for AdImageGetAndSubscribeManager to join


    int ret = 0;
    bool successfullyStartedWtServer = false;
    if((successfullyStartedWtServer = wtServer.start()))
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

    //tell AdImageGetAndSubscribeManager to finish. sets flags to not do any more operations, and lets current operations finish cleanly and then raises a wait condition when all current are done. It does NOT stop/destroy AdImageGetAndSubscribeManager completely, SINCE THE WT SERVER IS STILL RUNNING (and therefore using them (even though when it does use one, it gets instantly rejected because of the bool flags))
    QMetaObject::invokeMethod(&HackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "beginStopping", Qt::BlockingQueuedConnection);

    //then:
    if(successfullyStartedWtServer)
        wtServer.stop();

    QMetaObject::invokeMethod(&HackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "finishStopping", Qt::BlockingQueuedConnection);

    delete noAdPlaceholderImageResource;

    if(ret == 0)
    {
        cout << "HackyVideoBullshitSite Now Exitting Cleanly" << endl;
    }
    else
    {
        cout << "HackyVideoBullshitSite NOT exitting cleanly, return code: " << ret << endl;
    }
    return ret;
    //rest of cleanup is done in the scoped structs
}
WApplication *HackyVideoBullshitSite::hackyVideoBullshitSiteGuiEntryPoint(const WEnvironment &env)
{
    return new HackyVideoBullshitSiteGUI(env);
}
