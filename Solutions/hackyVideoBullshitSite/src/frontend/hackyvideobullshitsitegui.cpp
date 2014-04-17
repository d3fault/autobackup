#include "hackyvideobullshitsitegui.h"

#include <Wt/WServer>
#include <Wt/WVideo>
#include <Wt/WImage>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WLink>
#include <Wt/WEnvironment>
#include <Wt/WFileResource>
#include <Wt/WPushButton>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDate>

#define HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER "/some/placeholder/video/TODOreq.ogg" //TODOoptional: when no year/day folders are present (error imo) i could add code to present this... and it could even server as a pre-launch kind... of... countdown... thingo... (nah (ok changed my mind, yah (since it was a simple patch 'if year == 2013' xD)))
#define HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Videos/Airborne"

//segfault if server is started before assigning these that are pointers :-P (fuck yea performance)
AdImageGetAndSubscribeManager* HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = 0;
QString HackyVideoBullshitSiteGUI::m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL = QDir::rootPath(); //root tends to have a small number of files that rarely change, so a good default for when the user forgets to specify (since it will be 'watched')

void HackyVideoBullshitSiteGUI::setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager)
{
    m_AdImageGetAndSubscribeManager = adImageGetAndSubscribeManager;
}
void HackyVideoBullshitSiteGUI::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL)
{
    m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL = airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL;
}
HackyVideoBullshitSiteGUI::HackyVideoBullshitSiteGUI(const WEnvironment &env)
    : WApplication(env), m_AdImageAnchor(0), m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred(!env.ajax())
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "getAndSubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, static_cast<AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*>(this)), Q_ARG(std::string, sessionId()), Q_ARG(GetAndSubscriptionUpdateCallbackType, boost::bind(&HackyVideoBullshitSiteGUI::handleAdImageChanged, this, _1, _2, _3)));

    WContainerWidget *container = new WContainerWidget(root());
    container->setContentAlignment(Wt::AlignCenter | Wt::AlignTop);

    m_AdImagePlaceholderContainer = new WContainerWidget(container);
    m_AdImagePlaceholderContainer->setContentAlignment(Wt::AlignCenter | Wt::AlignTop);

    new WBreak(container);

    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred) //design-wise, the setting of this to true should be inside the body of this if. fuck it
    {
        deferRendering();
    }
    else
    {
        enableUpdates(true);
    }

    const std::string theInternalPathStdString = internalPath();
    if(theInternalPathStdString == "/" || theInternalPathStdString == "" || theInternalPathStdString == HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Latest")
    {
        std::string latestVideoSegmentFilePath = determineLatestVideoSegmentPathOrUsePlaceholder();
        //latestVideoSegmentFilePath is either set to most recent or to placeholder
        //WVideo(WFileResource) dat shit. TODOreq: download video file button

        //TODOreq: previous button
        WPushButton *nextVideoClipPushButton = new WPushButton("Next Clip", container); //if next != current; aka if new-current != current-when-started-playing
        //TODOreq: changing internal paths deletes/zeros button
        nextVideoClipPushButton->clicked().connect(this, &HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked);

        WVideo *videoPlayer = new WVideo(container); //TODOreq: changing paths (archive browsing, etc) deletes/zeros videoPlayer
        WFileResource *latestVideoSegmentFileResource = new WFileResource("video/ogg", latestVideoSegmentFilePath, videoPlayer);
        videoPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
        videoPlayer->addSource(WLink(latestVideoSegmentFileResource), "video/ogg");
        videoPlayer->resize(720, 480);
        //videoPlayer->resize(800, 600);
        videoPlayer->setAlternativeContent(new WText("Either your browser is a piece of shit and doesn't support HTML5 Video (You should use Mozilla Firefox), or there was an error establishing a connection to the video stream."));
        if(!environment().ajax())
        {
            return;
        }
        videoPlayer->ended().connect(this, &HackyVideoBullshitSiteGUI::handleLatestVideoSegmentEnded);
        return;
    }

    QString theInternalPathQString = QString::fromStdString(internalPath());
    if(theInternalPathQString.startsWith(HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/")) //this might (SHOULD!) be same code as "archive browsing"
    {

    }
    else
    {
        //TODOreq: MyBrain archive browsing, 404'ing.
        //TODOreq: post launch files should have a drop watch folder thingo too i suppose...
        //TODOreq: don't allow "../../../../" all the way up to root system folder hahaha, but i do still want to do my archive browsing based on the url supplied by user (thought about caching the URLs, but i'd just be duplicating what the filesystem buffer cache already does...)
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
        if(!environment().ajax())
        {
            return;
        }
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

    if(!environment().ajax())
    {
        //no-js will see the ad image update if/when they refresh or click a link to another item (TODOreq: test/confirm this, because i'm not sure it's true). COULD use a timer (which i'm more sure will work), but eh... naw..
        //^if you actually do any code for this, should probably copy/paste it to the no ad placeholder part too
        return;
    }
    triggerUpdate();
}
void HackyVideoBullshitSiteGUI::handleLatestVideoSegmentEnded()
{
    //TODOreq:
}
void HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked()
{

}
string HackyVideoBullshitSiteGUI::determineLatestVideoSegmentPathOrUsePlaceholder()
{
#if 0 //OTHER_SOLUTION_TO_SORTING_PROBLEM_MENTIONED_IN_COMMENTS_FOUND_LOL_WOOT
    {
        QDir videoSegmentsRootDir("/run/shm/butts");
        QStringList yearsDirListing = QDir::entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name); //TODOoptimization: even though the disk buffer cache saves us a hdd read, i'm willing to bet that the sorted results aren't cached. the further along in time i go (years wise), and also the further along int the year i get, and also the further along in the day i get, the more memory and cpu usage needed to re-sort over and over and over. so yea uhh solve that problem (unless it isn't a problem!). but for now KISS :-P
        QDir latestVideoSegmentYearDir(videoSegmentsRootDir.absolutePath() + QDir::separator() + yearsDirListing.first()); //it is definitely an optimization to just use QDateTime to find the year folder, BUT it might not exist if we've transitioned years but not yet moved
    }
#endif
    QDate currentDate = QDate::currentDate(); //WDate seems more natural given the context, but I doubt it matters.... AND Qt would probably be the faster of the two (or they're identical (this comment not...))
    for(;;) //i think this is faster than while(true) because there's no testing involved, but really i'd bet the compiler optimizes that out, in which case i like the look and readability of while(true) better (forever is just as schmexy (tried using it here but i have no_keywords shit going on guh))
    {
        bool yearFolderFound = QFile::exists(m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL + QString::number(currentDate.year())); //woot sorting problem solved for root folder ls SORT'ing
        if(!yearFolderFound)
        {
            currentDate = currentDate.addYears(-1);
            if(currentDate.year() == 2013) //can't time travel...
            {
                return HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER;
            }
            continue; //TODOreq: make sure video segment uploader is started and has uploaded at least one segment (and our VideoSegmentsImporterFolderWatcher has to have imported it and created relevant folders, OTHERWISE this will spinlock until int wraps around from negatives and finally does see the folder existing rofl. will do that for every WApplication too...
        }
        //year folder found, now find day folder using same method
        for(;;)
        {
            const QString &dayFolderToLookFor = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL + QString::number(currentDate.year()) + QDir::separator() + QString::number(currentDate.dayOfYear()) + QDir::separator();
            bool dayFolderFound = QFile::exists(dayFolderToLookFor); //TODOreq: as we are subtracting years, would it maybe make dayOfYear change as well in the leap year case (whoever invented leap years should be shot)
            if(!dayFolderFound)
            {
                currentDate = currentDate.addDays(-1);
                continue;
            }
            //day folder found, now find latest segment using sorting :(
            QDir dayFolder(dayFolderToLookFor);
            const QStringList all3MinuteSegmentsInDayFolder = dayFolder.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name | QDir::Reversed);
            return all3MinuteSegmentsInDayFolder.isEmpty() ? HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER /*perhaps should also prevent the year/day folder spinlock described above */ : (dayFolderToLookFor + all3MinuteSegmentsInDayFolder.first()).toStdString(); //TODOoptimization: read the if'd out stuff above about fixing having to list (cached.) + sort (probably not cached?) the dir. basically ~11:59pm will be more expensive than ~12:01am
        }
    }
    //should never get here, but just in case and to make compiler stfu:
    return HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER;
}
