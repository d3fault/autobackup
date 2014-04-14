#ifndef HACKYVIDEOBULLSHITSITEGUI_H
#define HACKYVIDEOBULLSHITSITEGUI_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include "../backend/adimagegetandsubscribemanager.h"

class QAtomicInt;

using namespace Wt;

class HackyVideoBullshitSiteGUI : public WApplication, public AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier
{
public:
    HackyVideoBullshitSiteGUI(const WEnvironment &env);
    ~HackyVideoBullshitSiteGUI();

    static AdImageGetAndSubscribeManager* m_AdImageGetAndSubscribeManager;
    static QAtomicInt *m_SharedVideoSegmentsArrayIndex;
    static std::string *(*m_SharedVideoSegmentsArray)[SHARED_VIDEO_SEGMENTS_ARRAY_SIZE]; //pointer to array of size X of pointer to std::string
private:
    WContainerWidget *m_AdImagePlaceholderContainer;
    WAnchor *m_AdImageAnchor; //so angry
    bool m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred;

    void handleAdImageChanged(WResource *newAdImageResource, std::string newAdUrl, std::string newAdAltAndHover);
};

#endif // HACKYVIDEOBULLSHITSITEGUI_H
