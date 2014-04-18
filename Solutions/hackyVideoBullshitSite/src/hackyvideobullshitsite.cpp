#include "hackyvideobullshitsite.h"

#include <Wt/WServer>

#include <fstream>

#include <QCoreApplication>
#include <QThread>
#include <QSettings>

#include "hackyvideobullshitsitebackendscopeddeleter.h"
#include "backend/adimagewresource.h"
#include "frontend/hackyvideobullshitsitegui.h"

#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch "VideoSegmentsImporterFolderToWatch"
#define HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace "VideoSegmentsImporterFolderScratchSpace"
#define HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo "AirborneVideoSegmentsBaseDir_aka_videoSegmentsImporterFolderToMoveTo"
#define HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir "MyBrainArchiveBaseDir"

int HackyVideoBullshitSite::startHackyVideoBullshitSiteAndWaitForFinished(int argc, char *argv[])
{
    //QCoreApplication::setOrganizationName("HackyVideoBullshitSiteOrganization");
    //QCoreApplication::setOrganizationDomain("HackyVideoBullshitSiteDomain.tld");
    //QCoreApplication::setApplicationName("HackyVideoBullshitSite");
    //QSettings::setDefaultFormat(QSettings::IniFormat);

    QCoreApplication qapp(argc, argv); //not sure if this is necessary (I've seen various documentation saying "can't call this until QCoreApplication is instantiated")... but probably won't hurt. I know I don't need to call qapp.exec at least

    QString placeholderPathForEdittingInSettings = "/-!-!-!-!-!-!-!-placeholder/path-!-!-!-!-!-!-!";
    QString videoSegmentsImporterFolderToWatch = placeholderPathForEdittingInSettings;
    QString videoSegmentsImporterFolderScratchSpace = placeholderPathForEdittingInSettings;
    QString airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = placeholderPathForEdittingInSettings;
    QString myBrainArchiveBaseDir_NoSlashAppended = placeholderPathForEdittingInSettings;

    {
        QSettings hackyVideoBullshitSiteSettings(QSettings::IniFormat, QSettings::UserScope, "HackyVideoBullshitSiteOrganization", "HackyVideoBullshitSite"); //because too lazy to integrate with Wt args... TODOoptional yea do it because editing settings files is teh suck (especially for automation). Still, future proofing for that by passing the settings by args to child objects instead of the static "setX" way of using qsettings (commented out above)

        bool atLeastOneDidntExist = false;

        videoSegmentsImporterFolderToWatch = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, placeholderPathForEdittingInSettings).toString());
        if(videoSegmentsImporterFolderToWatch == placeholderPathForEdittingInSettings)
        {
            cout << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderToWatch, videoSegmentsImporterFolderToWatch);
        }

        videoSegmentsImporterFolderScratchSpace = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, placeholderPathForEdittingInSettings).toString());
        if(videoSegmentsImporterFolderScratchSpace == placeholderPathForEdittingInSettings)
        {
            cout << "error: " HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_VideoSegmentsImporterFolderScratchSpace, videoSegmentsImporterFolderScratchSpace);
        }

        airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo = appendSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, placeholderPathForEdittingInSettings).toString());
        if(airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo == placeholderPathForEdittingInSettings)
        {
            cout << "error: " HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);
        }

        myBrainArchiveBaseDir_NoSlashAppended = removeTrailingSlashIfNeeded(hackyVideoBullshitSiteSettings.value(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, placeholderPathForEdittingInSettings).toString());
        if(myBrainArchiveBaseDir_NoSlashAppended == placeholderPathForEdittingInSettings)
        {
            cout << "error: " HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir " not set" << endl;
            atLeastOneDidntExist = true;
            hackyVideoBullshitSiteSettings.setValue(HackyVideoBullshitSite_SETTINGS_KEY_MyBrainArchiveBaseDir, myBrainArchiveBaseDir_NoSlashAppended);
        }

        if(atLeastOneDidntExist)
        {
            cout << "exitting with error because the settings in '" + hackyVideoBullshitSiteSettings.fileName().toStdString() + "' were not properly defined. look for the string '" + placeholderPathForEdittingInSettings.toStdString() + "' and fill in the proper paths. also look above for a more specific error" << endl;
            return 1;
        }
    }

    //start ad image get and subscribe thread and wait for it to finish initializing
    HackyVideoBullshitSiteBackendScopedDeleter hackyVideoBullshitSiteBackendScopedDeleterInstance(videoSegmentsImporterFolderToWatch, videoSegmentsImporterFolderScratchSpace, airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);

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

    QMetaObject::invokeMethod(&hackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "initializeAndStart", Qt::BlockingQueuedConnection);

    //AdImageGetAndSubscribeManager is done initializing, so now we set up Wt and then start the Wt server

    HackyVideoBullshitSiteGUI::setAdImageGetAndSubscribeManager(hackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend.adImageGetAndSubscribeManager());
    HackyVideoBullshitSiteGUI::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo);
    HackyVideoBullshitSiteGUI::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(myBrainArchiveBaseDir_NoSlashAppended);
    //OLD-when-atomic: similarly, instead of a pointer to a video segment file, i could point to a plugin to expand capability without shutting the server down :-P

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
    QMetaObject::invokeMethod(&hackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "beginStopping", Qt::BlockingQueuedConnection);

    //then:
    if(successfullyStartedWtServer)
        wtServer.stop();

    QMetaObject::invokeMethod(&hackyVideoBullshitSiteBackendScopedDeleterInstance.m_HackyVideoBullshitSiteBackend, "finishStopping", Qt::BlockingQueuedConnection);

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
