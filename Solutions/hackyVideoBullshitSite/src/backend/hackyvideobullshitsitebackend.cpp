#include "hackyvideobullshitsitebackend.h"

HackyVideoBullshitSiteBackend::HackyVideoBullshitSiteBackend(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &lastModifiedTimestampsFile, QAtomicPointer<LastModifiedTimestampsAndPaths> *timestampsAndPathsSharedAtomicPointer, QObject *parent)
    : QObject(parent)
    , m_AdImageGetAndSubscribeManager(0)
    , m_VideoSegmentsImporterFolderWatcherThread(0)
    , m_LastModifiedTimestampsWatcherThread(0)
    , m_VideoSegmentsImporterFolderToWatch(videoSegmentsImporterFolderToWatch)
    , m_VideoSegmentsImporterFolderScratchSpace(videoSegmentsImporterFolderScratchSpace)
    , m_VideoSegmentsImporterFolderToMoveTo(videoSegmentsImporterFolderToMoveTo)
    , m_LastModifiedTimestampsFile(lastModifiedTimestampsFile)
    , m_TimestampsAndPathsSharedAtomicPointer(timestampsAndPathsSharedAtomicPointer)
{ }
HackyVideoBullshitSiteBackend::~HackyVideoBullshitSiteBackend()
{
    finishStopping();
}
AdImageGetAndSubscribeManager *HackyVideoBullshitSiteBackend::adImageGetAndSubscribeManager()
{
    return m_AdImageGetAndSubscribeManager;
}
void HackyVideoBullshitSiteBackend::initializeAndStart()
{
    if(m_AdImageGetAndSubscribeManager)
        delete m_AdImageGetAndSubscribeManager;
    m_AdImageGetAndSubscribeManager = new AdImageGetAndSubscribeManager(this);

    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "initializeAndStart"); //if m_AdImageGetAndSubscribeManager is ever moved to [yet another] different thread, this [PROBABLY(because that's how i coded it before, but honestly i haven't verified it's necessary)] needs to be BlockingQueuedConnection. meh i've said that before too, perhaps i shouuld just fucking implement it into qt... (AutoConnectionBlockingQueuedIfNotSameThread... or BlockingAutoConnection). this comment is making me set up qt/gerrit comitting for first time ever (including compiling qt for first time ever (well, i did once before on freebsd for fun, but ran out of hdd space before it completed xD)


    ObjectOnThreadHelperBase::synchronouslyStopObjectOnThreadHelperIfNeeded(m_VideoSegmentsImporterFolderWatcherThread);
    m_VideoSegmentsImporterFolderWatcherThread = new ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher>(this);
    connect(m_VideoSegmentsImporterFolderWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleVideoSegmentsImporterFolderWatcherReadyForConnections()));
    m_VideoSegmentsImporterFolderWatcherThread->start(); //we don't need to wait for it to start before our own initializeAndStart is considered complete (unlike the AdImageGetAndSubscribeManager)

    ObjectOnThreadHelperBase::synchronouslyStopObjectOnThreadHelperIfNeeded(m_LastModifiedTimestampsWatcherThread);
    m_LastModifiedTimestampsWatcherThread = new ObjectOnThreadHelper<LastModifiedTimestampsWatcher>(this);
    connect(m_LastModifiedTimestampsWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLastModifiedTimestampsWatcherReadyForConnections()));
    m_LastModifiedTimestampsWatcherThread->start();
}
void HackyVideoBullshitSiteBackend::handleVideoSegmentsImporterFolderWatcherReadyForConnections()
{
    VideoSegmentsImporterFolderWatcher *videoSegmentsImporterFolderWatcher = m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly();
    //connect(videoSegmentsImporterFolderWatcher, &VideoSegmentsImporterFolderWatcher::d, this, &HackyVideoBullshitSiteBackend::d); //hmm now d seems too close to d_ptr xD
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(this, SIGNAL(finishStoppingRequested()), videoSegmentsImporterFolderWatcher, SLOT(finishStopping())); //TODOreq: should maybe be blocking queued, but i don't think that's necessary since i think QThread::quit is an event and that means that it won't be processed until after finishStopping... finishes. now terminate on the other hand is a different story :-P
    QMetaObject::invokeMethod(videoSegmentsImporterFolderWatcher, "initializeAndStart", Q_ARG(QString, m_VideoSegmentsImporterFolderToWatch), Q_ARG(QString, m_VideoSegmentsImporterFolderScratchSpace), Q_ARG(QString, m_VideoSegmentsImporterFolderToMoveTo));
}
void HackyVideoBullshitSiteBackend::handleLastModifiedTimestampsWatcherReadyForConnections()
{
    LastModifiedTimestampsWatcher *lastModifiedTimestampsWatcher = m_LastModifiedTimestampsWatcherThread->getObjectPointerForConnectionsOnly();
    connect(lastModifiedTimestampsWatcher, SIGNAL(e(QString)), this, SIGNAL(d(QString)));
    connect(this, SIGNAL(finishStoppingRequested()), lastModifiedTimestampsWatcher, SLOT(finishStopping()));
    QMetaObject::invokeMethod(lastModifiedTimestampsWatcher, "startWatchingLastModifiedTimestampsFile", Q_ARG(QString, m_LastModifiedTimestampsFile), Q_ARG(QAtomicPointer<LastModifiedTimestampsAndPaths>*, m_TimestampsAndPathsSharedAtomicPointer));
}
void HackyVideoBullshitSiteBackend::beginStopping()
{
    if(m_AdImageGetAndSubscribeManager)
        QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "beginStopping"); //ditto about blocking queued above
}
//safe to call twice
void HackyVideoBullshitSiteBackend::finishStopping()
{
    if(m_AdImageGetAndSubscribeManager)
    {
        QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "finishStopping"); //ditto about blocking queued above
        delete m_AdImageGetAndSubscribeManager;
        m_AdImageGetAndSubscribeManager = 0;
    }

    //tell our two backend threads to finish stopping
    emit finishStoppingRequested();

    //now wait on them and delete them (TO DOneoptimization: don't quit->wait, quit->wait, better to quit, quit, wait, wait)
    //ObjectOnThreadHelper::synchronouslyStopObjectOnThreadHelperIfNeeded(m_VideoSegmentsImporterFolderWatcherThread);
    //ObjectOnThreadHelper::synchronouslyStopObjectOnThreadHelperIfNeeded(m_LastModifiedTimestampsWatcherThread);
    ObjectOnThreadHelperBase::stopThreadIfNeeded(m_VideoSegmentsImporterFolderWatcherThread); //premature optimization is my boner
    ObjectOnThreadHelperBase::stopThreadIfNeeded(m_LastModifiedTimestampsWatcherThread);
    ObjectOnThreadHelperBase::waitOnAndDeleteThreadIfNeeded(m_VideoSegmentsImporterFolderWatcherThread);
    ObjectOnThreadHelperBase::waitOnAndDeleteThreadIfNeeded(m_LastModifiedTimestampsWatcherThread);
}
