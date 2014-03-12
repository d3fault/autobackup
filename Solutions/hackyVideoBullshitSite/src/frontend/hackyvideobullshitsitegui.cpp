#include "hackyvideobullshitsitegui.h"

#include <Wt/WVideo>
#include <Wt/WAudio>
#include <Wt/WImage>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WLink>

AdImageGetAndSubscribeManager* HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = 0;

HackyVideoBullshitSiteGUI::HackyVideoBullshitSiteGUI(const WEnvironment &env)
    : WApplication(env), m_AdImageAnchor(0)
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "getAndSubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, static_cast<AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*>(this)), Q_ARG(std::string, sessionId()), Q_ARG(GetAndSubscriptionUpdateCallbackType, boost::bind(&HackyVideoBullshitSiteGUI::handleAdImageChanged, this, _1, _2, _3)));

    WContainerWidget *container = new WContainerWidget(root());
    container->setContentAlignment(Wt::AlignCenter);

    m_AdImagePlaceholderContainer = new WContainerWidget(container);
    m_AdImagePlaceholderContainer->setContentAlignment(Wt::AlignCenter);

    new WBreak(container);
    new WBreak(container);
    new WBreak(container);

    //Checkbox: "Audio", checked by default

    //Radio: "Video" /// "Auto" (default), "Virtual Only" (screen), "Actual Only" (face), "Both". Auto is controlled by me through some manual mechanism (propagates via WServer::post). If I'm codan etc, screen. If not, face. Actually this sounds like too much of a pain in the ass for me to do manually, but idk what to default to otherwise xD (defaulting to "both" sounds good, but then I've just doubled my bandwidth rofl!)

    WVideo *videoPlayer = new WVideo(container);
    videoPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
    videoPlayer->addSource(WLink(WLink::Url, "http://localhost:8080/video.ogv"), "video/ogg");
    videoPlayer->resize(640, 480);
    videoPlayer->setAlternativeContent(new WText("Either your browser doesn't support HTML5 video, or there was an error establishing a connection to the video stream"));

    new WBreak(container); //is WAudio even visual?
    new WBreak(container);

    WAudio *audioPlayer = new WAudio(container);
    audioPlayer->setOptions(WAbstractMedia::Autoplay);
    audioPlayer->addSource(WLink(WLink::Url, "http://localhost:8081/audio.opus"), "audio/opus");
    audioPlayer->setAlternativeContent(new WText("Either your browser doesn't support HTML5 audio, or there was an error establishing a connection to the audio stream"));

    enableUpdates(true);
}
HackyVideoBullshitSiteGUI::~HackyVideoBullshitSiteGUI()
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "unsubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, this));
}
//TODOoptional: serialized wt instructions/etc passed in here and i could grow/transform the entire site before their eyes (has tons of WOO factor (but tbh, 'the web' (http/www) is fucking boring/shit), with the video being the only constant factor. i don't know the 'best' way to do what i describe, but i could do it hackily and i'm sure there's a 'proper' way to do it as well (would make for interesting reading)...
void HackyVideoBullshitSiteGUI::handleAdImageChanged(WResource *newAdImageResource, string newAdUrl, string newAdAltAndHover)
{
    if(m_AdImageAnchor)
        delete m_AdImageAnchor; //delete/replace the previous one, if there was one (TODOoptimization: maybe just setImage/setUrl/etc instead?)

    if(newAdUrl == "n") //hack. both newAdImageResource and newAdAltAndHover are undefined if url is "n" (they may be 0/empty, or they may still be 'yesterdays')
    {
        //set up "no ad" placeholder
        WImage *placeholderAdImage = new WImage(WLink(WLink::Url, /*http://d3fault.net*/"/no.ad.placeholder.jpg"), "Buy this ad space for BTC 0.00001");
        placeholderAdImage->resize(576, 96);
        m_AdImageAnchor = new WAnchor(WLink(WLink::Url, "http://anonymousbitcoincomputing.com/advertising/buy-ad-space/d3fault/0"), placeholderAdImage, m_AdImagePlaceholderContainer); //TODOreq: still need a link like that below/around the ad when purchased as well
        m_AdImageAnchor->setTarget(TargetNewWindow);
        placeholderAdImage->setToolTip("Buy this ad space for BTC 0.00001");
        m_AdImageAnchor->setToolTip("Buy this ad space for BTC 0.00001");
        triggerUpdate();
        return;
    }

    //TODOoptional: it would be trivial to show a 'price countdown' just like on abc2 just below the image itself (and has a wow factor of over 9000), but actually UPDATING that value on new purchases would NOT be [as] trivial :-/... so fuck it

    WImage *adImage = new WImage(newAdImageResource, newAdAltAndHover);
    adImage->resize(576, 96); //TODOreq: share a define with Abc2. also reminds me (though a little off topic) the mime/header of the resource and also the expiration date
    m_AdImageAnchor = new WAnchor(WLink(WLink::Url, newAdUrl), adImage, m_AdImagePlaceholderContainer);
    m_AdImageAnchor->setTarget(TargetNewWindow);

    //difference between these two?
    adImage->setToolTip(newAdAltAndHover);
    m_AdImageAnchor->setToolTip(newAdAltAndHover);

    triggerUpdate();
}
