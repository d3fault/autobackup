#include "hackyvideobullshitsite.h"

#include <Wt/WServer>

#include <QCoreApplication>
#include <QSettings>
#include <QAtomicPointer>

#include "standardinputnotifier.h"

#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch "VideoSegmentsImporterFolderToWatch"
#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace "VideoSegmentsImporterFolderYearDayOfYearFoldersScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo "AirborneVideoSegmentsBaseDir_aka_videoSegmentsImporterFolderToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir "MyBrainArchiveBaseDir"
#define HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile "LastModifiedTimestampsFile"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteSftpUploadScratchSpace "NeighborPropagationRemoteSftpUploadScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationRemoteDestinationToMoveTo "NeighborPropagationRemoteDestinationToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_NeighborPropagationUserHostPathComboSftpArg "NeighborPropagationUserHostPathComboSftpArg"
#define HackyVideoBullshitSite_SETTINGS_KEY_SftpProcessPath "SftpProcessPath"
#define HVBS_MOST_LIKELY_SFTP_PATH "/usr/bin/sftp"

#define HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS "/-!-!-!-!-!-!-!-placeholder/path-!-!-!-!-!-!-!"

HackyVideoBullshitSite::HackyVideoBullshitSite(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_ArgC(argc)
    , m_ArgV(argv)
    , m_WtControllerAndStdOutOwnerThread(new ObjectOnThreadHelper<WtControllerAndStdOutOwner>(this))
    , m_AdImageGetAndSubscribeManagerThread(new ObjectOnThreadHelper<AdImageGetAndSubscribeManager>(this))
    , m_VideoSegmentsImporterFolderWatcherThread(new ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher>(this))
    , m_LastModifiedTimestampsWatcherThread(new ObjectOnThreadHelper<LastModifiedTimestampsWatcher>(this))
    , m_ObjectOnThreadSynchronizer(new ObjectOnThreadSynchronizer(this))
    , m_VideoSegmentsImporterFolderToWatch(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_VideoSegmentsImporterFolderScratchSpace(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_MyBrainArchiveBaseDir_NoSlashAppended(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
    , m_LastModifiedTimestampsFile(HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
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

        m_VideoSegmentsImporterFolderToWatch = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_VideoSegmentsImporterFolderToWatch == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_VideoSegmentsImporterFolderScratchSpace = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_VideoSegmentsImporterFolderScratchSpace == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_MyBrainArchiveBaseDir_NoSlashAppended = removeTrailingSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString());
        if(m_MyBrainArchiveBaseDir_NoSlashAppended == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
        }

        m_LastModifiedTimestampsFile = hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS).toString();
        if(m_LastModifiedTimestampsFile == HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS)
        {
            cerr << "error: " HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_LastModifiedTimestampsFile, HVBS_PLACEHOLDERPATHFOREDITTINGINSETTINGS);
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
void HackyVideoBullshitSite::handleWtControllerAndStdOutOwnerIsReadyForConnections()
{
    WtControllerAndStdOutOwner *wtControllerAndStdOutOwner = m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly();
    connect(this, SIGNAL(e(QString)), wtControllerAndStdOutOwner, SLOT(handleE(QString)));
    connect(this, SIGNAL(o(QString)), wtControllerAndStdOutOwner, SLOT(handleO(QString)));
    connect(wtControllerAndStdOutOwner, SIGNAL(fatalErrorDetected()), this, SLOT(handleFatalError()));
    connect(wtControllerAndStdOutOwner, SIGNAL(stopped()), QCoreApplication::instance(), SLOT(quit()));
}
void HackyVideoBullshitSite::handleVideoSegmentsImporterFolderWatcherReadyForConnections()
{
    VideoSegmentsImporterFolderWatcher *videoSegmentsImporterFolderWatcher = m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly();
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(videoSegmentsImporterFolderWatcher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
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
    WtControllerAndStdOutOwner::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(m_MyBrainArchiveBaseDir_NoSlashAppended);

    LastModifiedTimestampsWatcher *lastModifiedTimestampsWatcher = m_LastModifiedTimestampsWatcherThread->getObjectPointerForConnectionsOnly();
    connect(lastModifiedTimestampsWatcher, SIGNAL(startedWatchingLastModifiedTimestampsFile()), this, SLOT(handleWatchingLastModifiedTimestampsFileStarted()));

    //could have done both of these connections in their respective "ready for connections" slots, but i want to give ad image manager a slight headstart. it's an optimization to do so, but not necessary
    connect(this, SIGNAL(beginStoppingRequested()), m_AdImageGetAndSubscribeManagerThread->getObjectPointerForConnectionsOnly(), SLOT(beginStopping()));
    connect(this, SIGNAL(beginStoppingRequested()), m_WtControllerAndStdOutOwnerThread->getObjectPointerForConnectionsOnly(), SLOT(stop()));

    //these invokeMethods need to be after ALL backends are ready for connections, or at least until WtControllerAndStdOutOwner is ready for connections (since it is our e/o handler!), because by invoking them when in individual ready for connections might miss e/o signals from them
    QMetaObject::invokeMethod(m_VideoSegmentsImporterFolderWatcherThread->getObjectPointerForConnectionsOnly(), "initializeAndStart", Q_ARG(QString, m_VideoSegmentsImporterFolderToWatch), Q_ARG(QString, m_VideoSegmentsImporterFolderScratchSpace), Q_ARG(QString, m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo), Q_ARG(QString, m_NeighborPropagationRemoteSftpUploadScratchSpace), Q_ARG(QString, m_NeighborPropagationRemoteDestinationToMoveTo), Q_ARG(QString, m_NeighborPropagationUserHostPathComboSftpArg), Q_ARG(QString, m_SftpProcessPath));
    QMetaObject::invokeMethod(lastModifiedTimestampsWatcher, "startWatchingLastModifiedTimestampsFile", Q_ARG(QString, m_LastModifiedTimestampsFile));
}
void HackyVideoBullshitSite::handleStandardInput(const QString &line)
{
    QString lineToLower = line.toLower();
    if(lineToLower == "h")
    {
        emit o("Q - Quit");
        return;
    }
    if(lineToLower == "q")
    {
        emit o("starting to quit...");
        disconnect(m_StdIn, SIGNAL(standardInputReceivedLine(QString)));
        emit beginStoppingRequested(); //bleh good design would warrant a way to synchronize that all backends are done with beginStopping and ready for finishStopping. i overengineered this solution (it now does thread::quit shit), but also that's only a hypothetical enhancement, i don't need to wait for the backends to be ready for finish stopping (finish stopping no longer exists <3, is just destructor now)
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
}

