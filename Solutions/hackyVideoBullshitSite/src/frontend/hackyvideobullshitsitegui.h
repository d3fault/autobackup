#ifndef HACKYVIDEOBULLSHITSITEGUI_H
#define HACKYVIDEOBULLSHITSITEGUI_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include "../backend/adimagegetandsubscribemanager.h"

using namespace Wt;

class HackyVideoBullshitSiteGUI : public WApplication, public AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier
{
public:
    HackyVideoBullshitSiteGUI(const WEnvironment &env);
    ~HackyVideoBullshitSiteGUI();

    static AdImageGetAndSubscribeManager* m_AdImageGetAndSubscribeManager;
private:
    WContainerWidget *m_AdImagePlaceholderContainer;
    WAnchor *m_AdImageAnchor; //so angry
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
