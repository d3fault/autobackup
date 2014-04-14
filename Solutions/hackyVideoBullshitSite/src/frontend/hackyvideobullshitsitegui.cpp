#include "hackyvideobullshitsitegui.h"

#include <Wt/WVideo>
#include <Wt/WImage>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WLink>
#include <Wt/WEnvironment>

#include <QAtomicInt>
#include <QString>

#define WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Videos/Airborne"

//segfault if server is started before assigning these :-P (fuck yea performance)
AdImageGetAndSubscribeManager* HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = 0;
QAtomicInt* HackyVideoBullshitSiteGUI::m_SharedVideoSegmentsArrayIndex = 0;
//mindfuck, maybe i don't need to set it to zero though, std::string *(*m_SharedVideoSegmentsArray)[SHARED_VIDEO_SEGMENTS_ARRAY_SIZE];

HackyVideoBullshitSiteGUI::HackyVideoBullshitSiteGUI(const WEnvironment &env)
    : WApplication(env), m_AdImageAnchor(0), m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred(!env.ajax())
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

    //Radio: "Video" /// "Auto" (default), "Virtual Only" (screen), "Actual Only" (face), "Both". Auto is controlled by me through some manual mechanism (propagates via WServer::post). If I'm codan etc, screen. If not, face. Actually this sounds like too much of a pain in the ass for me to do manually, but idk what to default to otherwise xD (defaulting to "both" sounds good, but then I've just doubled my bandwidth rofl!). Another idea almost worth its own text file is to use my mouse and/or motion detector to determine whether or not to show face only, or screen with face as thumbnail (which can still be 'separated' via this radio). Detecting motion on a SCREEN is easy and lossless, so whenever there ISN'T motion, we maximize face and don't even need to show the screen. I've decided I'll implement this later, however...

    WVideo *videoPlayer = new WVideo(container);
    videoPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
    videoPlayer->addSource(WLink(WLink::Url, "http://localhost:8080/video.ogv"), "video/ogg");
    videoPlayer->resize(640, 480);
    videoPlayer->setAlternativeContent(new WText("Either your browser doesn't support HTML5 Video, or there was an error establishing a connection to the video stream"));

    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred) //design-wise, the setting of this to true should be inside the body of this if. fuck it
    {
        deferRendering();
    }
    else
    {
        enableUpdates(true);
    }

    QString theInternalPath = QString::fromStdString(internalPath());
    if(theInternalPath == "/" || theInternalPath == "" || theInternalPath.startsWith(WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/") || theInternalPath == WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS)
    {
        const int sharedVideoSegmentsArrayIndex = m_SharedVideoSegmentsArrayIndex->load();

    }
    else
    {
        //TODOreq: MyBrain archive browsing, 404'ing.
        //TODOreq: post launch files should have a drop watch folder thingo too i suppose...
    }
}
HackyVideoBullshitSiteGUI::~HackyVideoBullshitSiteGUI()
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "unsubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, this));
}
//TODOoptional: serialized wt instructions/etc passed in here and i could grow/transform the entire site before their eyes (has tons of WOO factor (but tbh, 'the web' (http/www) is fucking boring/shit), with the video being the only constant factor. i don't know the 'best' way to do what i describe, but i could do it hackily and i'm sure there's a 'proper' way to do it as well (would make for interesting reading). I've thought more on this just because it's mildly interesting, and I think dynamic/run-time plugins would be the best way to do it
void HackyVideoBullshitSiteGUI::handleAdImageChanged(WResource *newAdImageResource, string newAdUrl, string newAdAltAndHover)
{
    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred)
    {
        resumeRendering();
        m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred = false;
    }

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
        if(environment().ajax())
        {
            triggerUpdate();
        }
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

    if(environment().ajax())
    {
        triggerUpdate();
    }
    //else: no-js will see the ad image update if/when they refresh or click a link to another item (TODOreq: test/confirm this, because i'm not sure it's true). COULD use a timer (which i'm more sure will work), but eh... naw..
    //^if you actually do any code for this 'else', should probably copy/paste it to the no ad placeholder part too
}
