#ifndef HACKYVIDEOBULLSHITSITEGUI_H
#define HACKYVIDEOBULLSHITSITEGUI_H

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WHBoxLayout>

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
    static QString m_MyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended;
    static std::string m_CopyrightText;
    static std::string m_DplLicenseText;

    WContainerWidget *m_AdImageContainer; //such an angry image container
    WContainerWidget *m_RightSideOfHBoxLayout;
    WWidget *m_Contents;
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    WContainerWidget *m_TimelineAndDirectoryLogicalContainer;
    WContainerWidget *m_CookieCrumbContainerWidget;
    WStackedWidget *m_TimelineAndDirectoryBrowsingStack;
    TimeLineWtWidget *m_TimeLineMyBrainWidget;
    DirectoryBrowsingWtWidget *m_BrowseMyBrainDirWidget;

    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
    void handleLatestVideoSegmentEnded();
    void handleNextVideoClipButtonClicked();
    std::string determineLatestVideoSegmentPathOrUsePlaceholder();

    void createCookieCrumbsFromPath(/*const std::string &internalPathInclFilename*/);
    void newTimelineIfNeededAndBringToFront();
    void deleteTimelineAndDirectoryBrowsingStackIfNeeded();
    void hvbs404();
    void setMainContent(WWidget *contentToSet);
    inline WContainerWidget *blahRootRedirect() { return m_RightSideOfHBoxLayout; }
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
