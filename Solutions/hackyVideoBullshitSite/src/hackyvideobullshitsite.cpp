#include "hackyvideobullshitsite.h"

#include <Wt/WServer>

#include <QCoreApplication>
#include <QSettings>
#include <QAtomicPointer>

#include "standardinputnotifier.h"

#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch "VideoSegmentsImporterFolderToWatch"
#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace "VideoSegmentsImporterFolderYearDayOfYearFoldersScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo "AirborneVideoSegmentsBaseDir_aka_videoSegmentsImporterFolderToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_HvbsWebBaseDir "HvbsWebBaseDir"
#define HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile "LastModifiedTimestampsFile"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteSftpUploadScratchSpace "NeighborPropagationRemoteSftpUploadScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteDestinationToMoveTo "NeighborPropagationRemoteDestinationToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationUserHostPathComboSftpArg "NeighborPropagationUserHostPathComboSftpArg"
#define HackyVideoBullshitSite_SETTINGS_KEY_SftpProcessPath "SftpProcessPath"
#define HVBS_MOST_LIKELY_SFTP_PATH "/usr/bin/sftp"

#define HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS "/-!-!-!-!-!-!-!-placeholder/path-!-!-!-!-!-!-!"

//TODOoptional: beneath the video on the front page I could show the 'latest 100' docs... either inline'd or just linked
HackyVideoBullshitSite::HackyVideoBullshitSite(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_ArgC(argc)
    , m_ArgV(argv)
    , m_WtControllerAndStdOutOwnerThread(new ObjectOnThreadHelper<WtControllerAndStdOutOwner>(this))
    , m_AdImageGetAndSubscribeManagerThread(new ObjectOnThreadHelper<AdImageGetAndSubscribeManager>(this))
    , m_VideoSegmentsImporterFolderWatcherThread(new ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher>(this))
    , m_LastModifiedTimestampsWatcherThread(new ObjectOnThreadHelper<LastModifiedTimestampsWatcher>(this))
    , m_ObjectOnThreadSynchronizer(new ObjectOnThreadSynchronizer(this))
    , m_StdIn(new StandardInputNotifier(this))
{
    qRegisterMetaType<char **>("char **");

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit())); //unneeded

    //QCoreApplication::setOrganizationName("HackyVideoBullshitSiteOrganization");
    //QCoreApplication::setOrganizationDomain("HackyVideoBullshitSiteDomain.tld");
    //QCoreApplication::setApplicationName("HackyVideoBullshitSite");
    //QSettings::setDefaultFormat(QSettings::IniFormat);

    {
        QSettings hackyVideoBullshitSiteSettings(QSettings::IniFormat, QSettings::UserScope, "HackyVideoBullshitSiteOrganization", "HackyVideoBullshitSite"); //because too lazy to integrate with Wt args... TODOoptional yea do it because editing settings files is teh suck (especially for automation). Still, future proofing for that by passing the settings by args to child objects instead of the static "setX" way of using qsettings (commented out above)

        bool atLeastOneDidntExist = false;

        int videoSegmentImportFoldersCount = hackyVideoBullshitSiteSettings.beginReadArray("VideoSegmentImportFolders");
        for(int i = 0; i < videoSegmentImportFoldersCount; ++i)
        {
            hackyVideoBullshitSiteSettings.setArrayIndex(i);
            QString oneVideoSegmentImportFolder = hackyVideoBullshitSiteSettings.value("AbsolutePathOfVideoSegmentImportFolder", HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
            if(oneVideoSegmentImportFolder == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
            {
                atLeastOneDidntExist = true;
                cerr << "error: please fill in all occurances of " HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS << endl;
            }
            m_VideoSegmentImporterFoldersToWatch.append(appendSlashIfNeeded(oneVideoSegmentImportFolder));
        }
        hackyVideoBullshitSiteSettings.endArray();
        if(m_VideoSegmentImporterFoldersToWatch.isEmpty()) //none? give them one sample to edit. TODOoptional: maybe make these optional instead of mandatory.. because the site doesn't hinge on videos being used (although in my case it does!). same with last modified timestamps
        {
            atLeastOneDidntExist = true;
            cerr << "error: please fill in all occurances of " HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS << endl;
            hackyVideoBullshitSiteSettings.beginWriteArray("VideoSegmentImportFolders");
            hackyVideoBullshitSiteSettings.setArrayIndex(0);
            hackyVideoBullshitSiteSettings.setValue("AbsolutePathOfVideoSegmentImportFolder", HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
            hackyVideoBullshitSiteSettings.endArray();
        }

        m_VideoSegmentsImporterFolderScratchSpace = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_VideoSegmentsImporterFolderScratchSpace == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }
        m_VideoSegmentsImporterFolderScratchSpace = appendSlashIfNeeded(m_VideoSegmentsImporterFolderScratchSpace);

        m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }
        m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = appendSlashIfNeeded(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);

        m_HvbsWebBaseDir_NoSlashAppended = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_HvbsWebBaseDir, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_HvbsWebBaseDir_NoSlashAppended == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_HvbsWebBaseDir " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_HvbsWebBaseDir, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }
        m_HvbsWebBaseDir_NoSlashAppended = removeTrailingSlashIfNeeded(m_HvbsWebBaseDir_NoSlashAppended);

        int lastModifiedTimestampFilesCount = hackyVideoBullshitSiteSettings.beginReadArray("LastModifiedTimestampFiles");
        for(int i = 0; i < lastModifiedTimestampFilesCount; ++i)
        {
            hackyVideoBullshitSiteSettings.setArrayIndex(i);
            QString oneLastModifiedTimestampsFile = hackyVideoBullshitSiteSettings.value("AbsoluteFilePathOfLastModifiedTimestampsFile", HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
            if(oneLastModifiedTimestampsFile == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
            {
                atLeastOneDidntExist = true;
                cerr << "error: please fill in all occurances of " HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS << endl;
            }
            m_LastModifiedTimestampsFiles.append(oneLastModifiedTimestampsFile);
        }
        hackyVideoBullshitSiteSettings.endArray();
        if(m_LastModifiedTimestampsFiles.isEmpty()) //none? give them one sample to edit
        {
            atLeastOneDidntExist = true;
            cerr << "error: please fill in all occurances of " HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS << endl;
            hackyVideoBullshitSiteSettings.beginWriteArray("LastModifiedTimestampFiles");
            hackyVideoBullshitSiteSettings.setArrayIndex(0);
            hackyVideoBullshitSiteSettings.setValue("AbsoluteFilePathOfLastModifiedTimestampsFile", HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
            hackyVideoBullshitSiteSettings.endArray();
        }

        m_NeighborPropagationRemoteSftpUploadScratchSpace = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteSftpUploadScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_NeighborPropagationRemoteSftpUploadScratchSpace == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteSftpUploadScratchSpace " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteSftpUploadScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_NeighborPropagationRemoteDestinationToMoveTo = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteDestinationToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_NeighborPropagationRemoteDestinationToMoveTo == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteDestinationToMoveTo " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteDestinationToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_NeighborPropagationUserHostPathComboSftpArg = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationUserHostPathComboSftpArg, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_NeighborPropagationUserHostPathComboSftpArg == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationUserHostPathComboSftpArg " not set" << endl;
            cerr << "if you use '" VideoSegmentsImporterFolderWatcher_DONT_PROPAGATE_TO_NEIGHBOR "' for the sftp host path combo arg, neighbor replication will be disabled" << endl; //TODOoptional: allow propagation enabling without bring the server down (aww shit it should have been a separate app and just used 'this app' "moveTo" as that one's "watch". over-engineered again) via cin/cout shit
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationUserHostPathComboSftpArg, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_SftpProcessPath = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_SftpProcessPath, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_SftpProcessPath == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            if(QFile::exists(HVBS_MOST_LIKELY_SFTP_PATH)) //auto-configya
            {
                cout << "warning: using " HVBS_MOST_LIKELY_SFTP_PATH " for sftp binary. you can specify another in the settings file" << endl;
                m_SftpProcessPath = HVBS_MOST_LIKELY_SFTP_PATH;
            }
            else
            {
                cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_SftpProcessPath " not set and sftp not found in default location" << endl;
                atLeastOneDidntExist = true;
            }
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_SftpProcessPath, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        if(atLeastOneDidntExist)
        {
            cerr << "exitting with error because the settings in '" + hackyVideoBullshitSiteSettings.fileName().toStdString() + "' were not properly defined. look for the string '" HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS "' and fill in the proper paths. also look above for a more specific error" << endl;
            handleFatalError();
            return;
        }
    }

    connect(m_WtControllerAndStdOutOwnerThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleWtControllerAndStdOutOwnerIsReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_WtControllerAndStdOutOwnerThread);
    m_WtControllerAndStdOutOwnerThread->start(); //yes that backend thread might start and emit readyForConnections, but 'this' thread won't process it until our constructor is finished! relevance = ObjectSynchronizer will only emit "all objects ready" after this unit of execution (typo was sex, sup nick), and the following addObjectToSynchronizer calls, are finished

    //don't have any that aren't dependent on the whole being ready?, simply don't connect :). connect(m_AdImageGetAndSubscribeManagerThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleAdImageGetAndSubscribeManagerIsReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_AdImageGetAndSubscribeManagerThread);
    m_AdImageGetAndSubscribeManagerThread->start();

    connect(m_VideoSegmentsImporterFolderWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleVideoSegmentsImporterFolderWatcherReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_VideoSegmentsImporterFolderWatcherThread);
    m_VideoSegmentsImporterFolderWatcherThread->start();

    connect(m_LastModifiedTimestampsWatcherThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLastModifiedTimestampsWatcherReadyForConnections()));
    m_ObjectOnThreadSynchronizer->addObjectToSynchronizer(m_LastModifiedTimestampsWatcherThread);
    m_LastModifiedTimestampsWatcherThread->start();

    connect(m_ObjectOnThreadSynchronizer, SIGNAL(allObjectsOnThreadsReadyForConnections()), this, SLOT(handleAllBackendObjectsOnThreadsReadyForConnections())); //death to boolean flags like 'm_XisReady' and BlockingQueuedConnection initialization!

    connect(m_StdIn, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInput(QString)));
}
void HackyVideoBullshitSite::cliUsage()
{
    QString cliUsageStr =   "Available Actions (H to show this again):\n"
                            " 1   - Query ffmpeg segment neighbor propagation status info, which includes:\n\t-Most recent segment entry\n\t-The size of the upload queue\n\t-The 'head' of the upload queue\n\t-The sftp connection status)\n"
                            " Q   - Stop HackyVideoBullshitSite and Quit after all segments are propagated to neighbor (sftp will retry indefinitely)\n"
                            " QQ  - Stop HackyVideoBullshitSite and Quit after all segments are propagated to neighbor, unless sftp connection is dead or dies beforehand\n"
                            " QQQ - Stop HackyVideoBullshitSite and Quit now (use Q or QQ if you can)\n";
    emit o(cliUsageStr);
}
void HackyVideoBullshitSite::handleWtControllerAndStdOutOwnerIsReadyForConnections()
{
    WtControllerAndStdOutOwner *wtControllerAndStdOutOwner = m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(e(QString)), wtControllerAndStdOutOwner, SLOT(handleE(QString)));
    connect(this, SIGNAL(o(QString)), wtControllerAndStdOutOwner, SLOT(handleO(QString)));
    connect(wtControllerAndStdOutOwner, SIGNAL(started()), this, SLOT(cliUsage()));
    connect(wtControllerAndStdOutOwner, SIGNAL(fatalErrorDetected()), this, SLOT(handleFatalError()));
    connect(wtControllerAndStdOutOwner, SIGNAL(stopped()), QCoreApplication::instance(), SLOT(quit()));
}
void HackyVideoBullshitSite::handleVideoSegmentsImporterFolderWatcherReadyForConnections()
{
    VideoSegmentsImporterFolderWatcher *videoSegmentsImporterFolderWatcher = m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly();
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(this, SIGNAL(tellVideoSegmentNeighborPropagationInformationRequested()), videoSegmentsImporterFolderWatcher, SIGNAL(tellNeighborPropagationInformationRequested())); //this seems backwards for some reason, i wonder if it'll work. if not, ez solution: make it a slot that emits the signal gg

    connect(this, SIGNAL(stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesRequested()), videoSegmentsImporterFolderWatcher, SLOT(stopCleanlyOnceVideoSegmentNeighborPropagatationFinishes()));
    connect(this, SIGNAL(stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDcRequested()), videoSegmentsImporterFolderWatcher, SLOT(stopCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDc()));
    connect(this, SIGNAL(stopHackyVideoBullshitSiteNowRequested()), videoSegmentsImporterFolderWatcher, SLOT(stopNow()));

    connect(videoSegmentsImporterFolderWatcher, SIGNAL(videoSegmentsImporterFolderWatcherFinishedPropagatingToNeighbors()), this, SIGNAL(videoSegmentNeighborPropagationFinishedStopContinueStoppingRequested())); //bleh good design would warrant a way to synchronize that all backends are done with beginStopping and ready for finishStopping. i overengineered this solution (it now does thread::quit shit), but also that's only a hypothetical enhancement, i don't need to wait for the backends to be ready for finish stopping (finish stopping no longer exists <3, is just destructor now)
}
void HackyVideoBullshitSite::handleLastModifiedTimestampsWatcherReadyForConnections()
{
    LastModifiedTimestampsWatcher *lastModifiedTimestampsWatcher = m_LastModifiedTimestampsWatcherThread->getObjectPointerForConnectionsOnly();
    connect(lastModifiedTimestampsWatcher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    //OLD-when-atomic: similarly, instead of a pointer to a video segment file, i could point to a plugin to expand capability without shutting the server down :-P
    //o hai atomicz <3 (*throws up*)
    WtControllerAndStdOutOwner::setTimestampsAndPathsSharedAtomicPointer(lastModifiedTimestampsWatcher->getTimestampsAndPathsAtomicPointer());
}
void HackyVideoBullshitSite::handleWatchingLastModifiedTimestampsFileStarted()
{
    QMetaObject::invokeMethod(m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly(), "initializeAndStart", Q_ARG(int, m_ArgC), Q_ARG(char **, m_ArgV));
}
//void HackyVideoBullshitSite::handleAdImageGetAndSubscribeManagerIsReadyForConnections()
//{
    //derp passes itself to WtControllerAndStdOutOwner, but we dunno if he's ready yet :-P (FUKKEN SOLVED FINALLY <3 <3 <3)
//}
void HackyVideoBullshitSite::handleAllBackendObjectsOnThreadsReadyForConnections()
{
    WtControllerAndStdOutOwner::setAdImageGetAndSubscribeManager(m_AdImageGetAndSubscribeManagerThread->getObjectPointerForConnectionsOnly());
    WtControllerAndStdOutOwner::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);
    WtControllerAndStdOutOwner::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(m_HvbsWebBaseDir_NoSlashAppended);

    LastModifiedTimestampsWatcher *lastModifiedTimestampsWatcher = m_LastModifiedTimestampsWatcherThread->getObjectPointerForConnectionsOnly();
    connect(lastModifiedTimestampsWatcher, SIGNAL(startedWatchingLastModifiedTimestampsFile()), this, SLOT(handleWatchingLastModifiedTimestampsFileStarted()));

    //could have done both of these connections in their respective "ready for connections" slots, but i want to give ad image manager a slight headstart. it's an optimization to do so, but not necessary
    connect(this, SIGNAL(videoSegmentNeighborPropagationFinishedStopContinueStoppingRequested()), m_AdImageGetAndSubscribeManagerThread->getObjectPointerForConnectionsOnly(), SLOT(beginStopping()));
    connect(this, SIGNAL(videoSegmentNeighborPropagationFinishedStopContinueStoppingRequested()), m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly(), SLOT(stop()));

    //these invokeMethods need to be after ALL backends are ready for connections, or at least until WtControllerAndStdOutOwner is ready for connections (since it is our e/o handler!), because by invoking them when in individual ready for connections might miss e/o signals from them
    QMetaObject::invokeMethod(m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly(), "initializeAndStart", Q_ARG(QStringList, m_VideoSegmentImporterFoldersToWatch), Q_ARG(QString, m_VideoSegmentsImporterFolderScratchSpace), Q_ARG(QString, m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo), Q_ARG(QString, m_NeighborPropagationRemoteSftpUploadScratchSpace), Q_ARG(QString, m_NeighborPropagationRemoteDestinationToMoveTo), Q_ARG(QString, m_NeighborPropagationUserHostPathComboSftpArg), Q_ARG(QString, m_SftpProcessPath));
    QString hvbsWebViewBaseDir = m_HvbsWebBaseDir_NoSlashAppended + "/view";
    QMetaObject::invokeMethod(lastModifiedTimestampsWatcher, "startWatchingLastModifiedTimestampsFiles", Q_ARG(QString, hvbsWebViewBaseDir), Q_ARG(QStringList, m_LastModifiedTimestampsFiles));
}
void HackyVideoBullshitSite::handleStandardInput(const QString &line)
{
    QString lineToLower = line.toLower();
    if(lineToLower == "h")
    {
        cliUsage();
        return;
    }
    if(lineToLower == "1")
    {
        emit tellVideoSegmentNeighborPropagationInformationRequested();
        return;
    }
    if(lineToLower == "q")
    {
        emit o("HackyVideoBullshitSite will quit once all video segment neighbor propagation is finished (sftp will retry infinitely)");
        emit stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesRequested();
        return;
    }
    if(lineToLower == "qq") //TODOreq: i think QQ might make HackyVideoBullshitSite never quit if ffmpeg segment uploader is still running and pumping HackyVideoBullshitSite segments. i think whether or not it is an issue depends on upload speed and whether or not the queue is ever empty. maybe i can do "only propagate ones i already have" mode, or maybe this is desired behavior and i should just warn about it
    {
        emit o("HackyVideoBullshitSite will quit once all video segment neighbor propagation is finished (unless the sftp connection is dead or dies)");
        emit stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDcRequested();
        return;
    }
    if(lineToLower == "qqq")
    {
        emit o("HackyVideoBullshitSite will quit now (neighbors will not not get latest video segments propagated to them)...");
        disconnect(m_StdIn, SIGNAL(standardInputReceivedLine(QString)));
        emit stopHackyVideoBullshitSiteNowRequested();
        return;
    }
}
void HackyVideoBullshitSite::handleAboutToQuit()
{
    //i'm too cool for this (unneeded)...
    //but let's assume that we didn't have faith in our destruction order and want to make sure m_ObjectOnThreadSynchronizer is destructed first, since not destructing it first will segfault us as it tries to call QThread::quit on... (O SHIT NVM THEY ARE SLOTS HAHA I AM DUMB I CAN CALL SLOTS SAFELY ALWAYS NO MATTER THE ORDER)
    if(m_ObjectOnThreadSynchronizer)
    {
        delete m_ObjectOnThreadSynchronizer; //not necessary because of parenting (is parenting lifo? i would guess yes... but idk)
        m_ObjectOnThreadSynchronizer = 0;
    }
}
void HackyVideoBullshitSite::handleFatalError()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
    //QCoreApplication::exit(1); //<-- PROOF THAT QUIT NEEDS TO BE QUEUED OR ELSE DEAD LOCK  at return qapp.exec(); (uncomment and comment above), WTF. especially wtf since i'm on gui thread. should submit patch :-P
    //^so apprently the reason is that the event loop has to already be running for exit/quit to work. dumb. there's no "return 1;" from constructor equivalent
}
