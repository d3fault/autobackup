#include "hackyvideobullshitsitebackend.h"

HackyVideoBullshitSiteBackend::HackyVideoBullshitSiteBackend(QAtomicInt *sharedVideoSegmentsArrayIndex, QObject *parent) :
    QObject(parent)
    , m_AdImageGetAndSubscribeManager(0)
    , m_VideoSegmentsImporterFolderWatcherThread(0)
    , m_SharedVideoSegmentsArrayIndex(sharedVideoSegmentsArrayIndex)
{ }
HackyVideoBullshitSiteBackend::~HackyVideoBullshitSiteBackend()
{
    if(m_AdImageGetAndSubscribeManager)
        delete m_AdImageGetAndSubscribeManager;

    endVideoSegmentsImporterFolderWatcherThreadIfNeeded();
}
AdImageGetAndSubscribeManager *HackyVideoBullshitSiteBackend::adImageGetAndSubscribeManager()
{
    return m_AdImageGetAndSubscribeManager;
}
void HackyVideoBullshitSiteBackend::endVideoSegmentsImporterFolderWatcherThreadIfNeeded()
{
    if(m_VideoSegmentsImporterFolderWatcherThread)
    {
        if(m_VideoSegmentsImporterFolderWatcherThread->isRunning())
            m_VideoSegmentsImporterFolderWatcherThread->quit();
        m_VideoSegmentsImporterFolderWatcherThread->wait();
        delete m_VideoSegmentsImporterFolderWatcherThread;
        m_VideoSegmentsImporterFolderWatcherThread = 0;
    }
}
void HackyVideoBullshitSiteBackend::initializeAndStart()
{
    if(m_AdImageGetAndSubscribeManager)
        delete m_AdImageGetAndSubscribeManager;
    m_AdImageGetAndSubscribeManager = AdImageGetAndSubscribeManager(this);

    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "initializeAndStart"); //if m_AdImageGetAndSubscribeManager is ever moved to [yet another] different thread, this [PROBABLY(because that's how i coded it before, but honestly i haven't verified it's necessary)] needs to be BlockingQueuedConnection. meh i've said that before too, perhaps i shouuld just fucking implement it into qt... (AutoConnectionBlockingQueuedIfNotSameThread... or BlockingAutoConnection). this comment is making me set up qt/gerrit comitting for first time ever (including compiling qt for first time ever (well, i did once before on freebsd for fun, but ran out of hdd space before it completed xD)


    if(m_VideoSegmentsImporterFolderWatcherThread)
        delete m_VideoSegmentsImporterFolderWatcherThread;
    m_VideoSegmentsImporterFolderWatcherThread = new ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher>(this);
    connect(m_VideoSegmentsImporterFolderWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleVideoSegmentsImporterFolderWatcherReadyForConnections()));
    m_VideoSegmentsImporterFolderWatcherThread->start(); //we don't need to wait for it to start before our own initializeAndStart is considered complete (unlike the AdImageGetAndSubscribeManager)
}
void HackyVideoBullshitSiteBackend::handleVideoSegmentsImporterFolderWatcherReadyForConnections()
{
    QMetaObject::invokeMethod(m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly(), "initializeAndStart", Q_ARG(QAtomicInt*,m_SharedVideoSegmentsArrayIndex)); //ditto about blocking queued above
}
void HackyVideoBullshitSiteBackend::beginStopping()
{
    if(m_AdImageGetAndSubscribeManager)
        QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "beginStopping"); //ditto about blocking queued above
}
void HackyVideoBullshitSiteBackend::finishStopping()
{
    if(m_AdImageGetAndSubscribeManager)
    {
        QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "finishStopping"); //ditto about blocking queued above
        delete m_AdImageGetAndSubscribeManager;
        m_AdImageGetAndSubscribeManager = 0;
    }

    endVideoSegmentsImporterFolderWatcherThreadIfNeeded();
}
