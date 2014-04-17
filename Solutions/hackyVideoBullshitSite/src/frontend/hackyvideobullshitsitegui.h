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

    HackyVideoBullshitSiteGUI(const WEnvironment &env);
    ~HackyVideoBullshitSiteGUI();
private:
    static AdImageGetAndSubscribeManager* m_AdImageGetAndSubscribeManager;
    static QString m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL;

    WContainerWidget *m_AdImagePlaceholderContainer;
    WAnchor *m_AdImageAnchor; //so angry
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
    void handleLatestVideoSegmentEnded();
    void handleNextVideoClipButtonClicked();
    std::string determineLatestVideoSegmentPathOrUsePlaceholder();
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
