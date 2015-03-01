#ifndef HACKYVIDEOBULLSHITSITEGUI_H
#define HACKYVIDEOBULLSHITSITEGUI_H

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WImage>
#include <Wt/WAnchor>

#include "../lastmodifiedtimestampsandpaths.h"
#include "../backend/adimagegetandsubscribemanager.h"

using namespace Wt;

class TimeLineWtWidget;
class DirectoryBrowsingWtWidget;

class HackyVideoBullshitSiteGUI : public WApplication, public AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier
{
public:
    static void setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager* adImageGetAndSubscribeManager);
    static void setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL);
    static void setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended);
    static void setCopyrightText(const std::string &copyrightText);
    static void setDplLicenseText(const std::string &dplLicenseText);
    static void setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer);
    static WApplication *hackyVideoBullshitSiteGuiEntryPoint(const WEnvironment &env);

    HackyVideoBullshitSiteGUI(const WEnvironment &env);
protected:
    virtual void finalize();
private:
    static AdImageGetAndSubscribeManager* m_AdImageGetAndSubscribeManager;
    static QString m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended;
    static QString m_HvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended;
    static std::string m_CopyrightText;
    static std::string m_DplLicenseText;

    WImage *m_AdImage;
    WAnchor *m_AdImageAnchor;
    WContainerWidget *m_MainContentsContainer;
    WWidget *m_Contents;
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    std::string m_VideoSegmentFilePathCurrentlyBeingDisplayed; //used to find "next" video
    bool m_AutoPlayNextVideoSegmentCheckboxChecked;
    WContainerWidget *m_TimelineAndDirectoryLogicalContainer;
    WContainerWidget *m_CookieCrumbContainerWidget;
    WStackedWidget *m_TimelineAndDirectoryBrowsingStack;
    TimeLineWtWidget *m_TimeLineMyBrainWidget;
    DirectoryBrowsingWtWidget *m_BrowseMyBrainDirWidget;

    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
    void displayVideoSegment(const std::string &videoSegmentFilePath);
    void handleAutoPlayNextVideoSegmentCheckboxChecked();
    void handleAutoPlayNextVideoSegmentCheckboxUnchecked();
    void handleLatestVideoSegmentEnded();
    QString getEarliestEntryInFolder(const QString &folderWithSlashAppended);
    void handleNextVideoClipButtonClicked();
    std::string determineLatestVideoSegmentPathOrUsePlaceholder();
    void tellUserThatCurrentVideoSegmentIsLatest();
    void handleHomeAnchorClickedSoShowLatestVideoSegmentEvenIfAlreadyHome();

    void createCookieCrumbsFromPath(/*const std::string &internalPathInclFilename*/);
    void newTimelineIfNeededAndBringToFront();
    void deleteTimelineAndDirectoryBrowsingStackIfNeeded();
    void hvbs404();
    void setMainContent(WWidget *contentToSet);
    inline WContainerWidget *blahRootRedirect() { return m_MainContentsContainer; }
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
