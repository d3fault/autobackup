#ifndef HACKYVIDEOBULLSHITSITEGUI_H
#define HACKYVIDEOBULLSHITSITEGUI_H

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include "../backend/adimagegetandsubscribemanager.h"

using namespace Wt;

class HackyVideoBullshitSiteGUI : public WApplication, public AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier
{
public:
    static void setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager* adImageGetAndSubscribeManager);
    static void setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL);
    static void setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended);

    HackyVideoBullshitSiteGUI(const WEnvironment &env);
    ~HackyVideoBullshitSiteGUI();
private:
    static AdImageGetAndSubscribeManager* m_AdImageGetAndSubscribeManager;
    static QString m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended;
    static QString m_MyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended;

    WContainerWidget *m_AdImagePlaceholderContainer;
    WContainerWidget *m_ContentPlaceholderContainer;
    WAnchor *m_AdImageAnchor; //so angry
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    void handleInternalPathChanged(const std::string &newInternalPath);
    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
    void handleLatestVideoSegmentEnded();
    void handleNextVideoClipButtonClicked();
    std::string determineLatestVideoSegmentPathOrUsePlaceholder();
    void handleDownloadButtonClicked();

    void embedBasedOnMimeType(const std::string &mimeType);
    void embedPicture(const std::string &mimeType, const QString &filename);
    void embedVideoFile(const std::string &mimeType, const QString &filename);
    void embedAudioFile(const std::string &mimeType, const QString &filename);

    string embedBasedOnFileExtensionAndReturnMimeType(const QString &filename);
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
