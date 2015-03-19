#include "hackyvideobullshitsitegui.h"

#include <Wt/WServer>
#include <Wt/WVideo>
#include <Wt/WText>
#include <Wt/WLink>
#include <Wt/WEnvironment>
#include <Wt/WFileResource>
#include <Wt/WPushButton>
#include <Wt/WPanel>
#include <Wt/Utils>
#include <Wt/WCssStyleSheet>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WCheckBox>
#include <Wt/WDateTime>
#include <Wt/WIntValidator>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDate>
#include <QDateTime>
#include <QFileInfo>

#include "hvbsshared.h"
#include "timelinewtwidget.h"
#include "directorybrowsingwtwidget.h"

#define HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER "/some/placeholder/video/TODOreq.ogg" //TODOoptional: when no year/day folders are present (error imo) i could add code to present this... and it could even server as a pre-launch kind... of... countdown... thingo... (nah (ok changed my mind, yah (since it was a simple patch 'if year == 2013' xD)))

#define HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_DOWNLOAD \
    HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER "/Download"

#define HVBS_WEB_CLEAN_URL_TO_STORE HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER "/Store"

#define HVBS_VIEW_MYBRAIN_STRING "Archive"

#define HVBS_TIMELINE_MY_BRAIN_STRING "Timeline (Random point in time)"
#define HVBS_BROWSE_MY_BRAIN_STRING "Browse"
#define HVBS_DOWNLOAD_MY_BRAIN_STRING "Full download for offline viewing"

#define HVBS_VIEW_MYBRAIN_TOOLTIP "The shit..."

#define HVBS_TIMELINE_MYBRAIN_TOOLTIP "In the beginning, there was d3fault..."
#define HVBS_BROWSE_MYBRAIN_TOOLTIP "Traditional directory heirarchy"
#define HVBS_DOWNLOAD_MYBRAIN_TOOLTIP "My brain mingles with your brain" // s/becomes/mingles-with/

#define HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL "https://anonymousbitcoincomputing.com/advertising/buy-ad-space/d3fault/0"

#define HVBS_ABC2_AD_IMAGE_WIDTH 576
#define HVBS_ABC2_AD_IMAGE_HEIGHT 96

#define HVBS_ABC2_PLACEHOLDER_ALT_AND_HOVER "Buy this ad space for BTC 0.00003"

#define HVBS_VIDEO_SEGMENT_SPECIAL_PATH_TO_INDICATE_CURRENT_IS_LATEST "n"

#define HVBS_TIMESTAMP_OF_VIDEO_LAST_WATCHED_COOKIE_KEY "hvbsTimestampOfVideoLastWatched"

//TODOoptional: to the right of the vid I could have "news" and/or [hand picked] "interesting clips"

//segfault if server is started before assigning these that are pointers :-P (fuck yea performance)
AdImageGetAndSubscribeManager* HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = 0;
QString HackyVideoBullshitSiteGUI::m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended = QDir::rootPath(); //root tends to have a small number of files that rarely change, so a good default for when the user forgets to specify (since it will be 'watched')
QString HackyVideoBullshitSiteGUI::m_HvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended = QDir::homePath(); //internal path is concatenated onto this, and already has a slash at the beginning
std::string HackyVideoBullshitSiteGUI::m_CopyrightText = "All Rights Reserved"; //the legal default, but don't interpret this as my liking it
std::string HackyVideoBullshitSiteGUI::m_DplLicenseText = "Geef monies and then you can express yourself without breaking the law and worrying about getting v&"; //kim dot com

void HackyVideoBullshitSiteGUI::setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager)
{
    m_AdImageGetAndSubscribeManager = adImageGetAndSubscribeManager;
}
void HackyVideoBullshitSiteGUI::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL)
{
    m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended = airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL;
}
void HackyVideoBullshitSiteGUI::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &hvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended)
{
    m_HvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended = hvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended;
    TimeLineWtWidget::setHvbsWebBaseDirActual(hvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended);
}
void HackyVideoBullshitSiteGUI::setCopyrightText(const string &copyrightText)
{
    m_CopyrightText = "<pre>" + Wt::Utils::htmlEncode(copyrightText) + "</pre>"; //old because only done at once startup now: TODOoptimization: could pre-htmlEncode and even color'ify (code) the various docs beforehand, instead of on-demand like i'm doing now (well, i'm not doing the coloring at all atm)
}
void HackyVideoBullshitSiteGUI::setDplLicenseText(const string &dplLicenseText)
{
    m_DplLicenseText = "<pre>" + Wt::Utils::htmlEncode(dplLicenseText) + "</pre>";
}
void HackyVideoBullshitSiteGUI::setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer)
{
    TimeLineWtWidget::setTimestampsAndPathsSharedAtomicPointer(lastModifiedTimestampsSharedAtomicPointer);
}
WApplication *HackyVideoBullshitSiteGUI::hackyVideoBullshitSiteGuiEntryPoint(const WEnvironment &env)
{
    return new HackyVideoBullshitSiteGUI(env);
}
HackyVideoBullshitSiteGUI::HackyVideoBullshitSiteGUI(const WEnvironment &env)
    : WApplication(env)
    , m_Contents(0)
    , m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred(!env.ajax())
    , m_AutoPlayNextVideoSegmentCheckboxChecked(true)
    , m_TimelineAndDirectoryLogicalContainer(0)
    , m_TimeLineMyBrainWidget(0)
    , m_BrowseMyBrainDirWidget(0)
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "getAndSubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, static_cast<AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*>(this)), Q_ARG(std::string, sessionId()), Q_ARG(GetAndSubscriptionUpdateCallbackType, boost::bind(&HackyVideoBullshitSiteGUI::handleAdImageChanged, this, _1, _2, _3)));

    setTitle("I've been living the past few years as if there were tiny cameras all around me. The only path to sanity is to make that definitely true");

    setCssTheme("polished");

    styleSheet().addRule("body", "background-color: black; color: white; font-family: arial; font-size: large");
    styleSheet().addRule("a:link", "color: #e1e1e1;");
    styleSheet().addRule("a:visited", "color: #ffa2a2;");
    styleSheet().addRule("pre", "white-space: pre-wrap; white-space: -moz-pre-wrap; white-space: -pre-wrap; white-space: -o-pre-wrap; word-wrap: break-word;"); //fucking hate css, not as much as js but (had:way) more than html (except when css saves the day from htmls shortcomings...). on that note, fuck the rule saying you can't use an apostrophe s unless it means "<something> is" (unless used with proper noun(?)). "htmls" looks fucking retarded. html owns the shortcomings, therefore posessive, who cares if it's not a proper noun (or is it? fuck if i know guh all subjective bullshit anyways. i'm going for clarity, suck my dick)
    //styleSheet().addRule("table, th, td", "border: 1px solid white;");
    styleSheet().addRule("img", "background-color: white;"); //for svg images, which are transparent and [i] tend to use black font...

    //TODOreq: text files downloaded copies have copyright.txt at top. I'm thinking my 'master' branch has copyright.txt prepend thing always at top, and another separate branch is what I work on (script prepends and merges/pushes/whatevers into master). master because it should be the default for anyone that checks it out... but i don't want to permanently put that shit at the top of mine because it will annoy the fuck out of me (especially since i don't want the EMBED copy to have it since it is way sexier to have it in a WPanel). It could be called the allrightsreserved branch xD. Note: not that it matters, but I think I'd need to be constantly creating a temporary branch, running the prepender, committing, and then... err... i think rebasing ONTO master? idfk lol... i suck at git
    //but also collaboration and merging etc will mean i have to deal with licenses anyways. for code i don't care that much tbh, but for text files that aren't even that long.... fuuuuuuck i don't want stupid headers prepended on all of em. BUT honestly they're easy to both insert and remove via scripting so... (lol at the bug where i 'remove' the text and then it removes it from the file that i used as input to tell me what to remove (easily fixed by pulling it back out of git history (or a skip file exception) but still i'm predicting it will happen :-P)

    //TODOreq: "Store" (lulu, cafepress, eventually "dvd/bd copy of archive", etc. research floss bitcoin store software (which probably won't be horizontally scalable -_-))
    //TODOreq: text repo readonly git:// clone url in download area (down below, not part of the 3). that repo should have copyright headers prepended? methinks yesh
    //TODOreq: "new node" script (might just use pre/post scripts as usual? except pre shouldn't launch hvbs. should wait for data to finish unpacking)

    //TODOmaybe: /licenses/ is a repository? a static public resource?

    //TODOoptional: if the browser isn't wide enough, my links to the right of the ad aren't shown (and can't be scrolled to wtf). WNavigationBar/Panel thing has a setResponsive(true) function that can turn things like that into a vertical popup when the screen shrinks too small
    //TODOoptional: folder (recursive) saving... but how would i do that, zip on demand? more importantly, how would i limit it?
    //TODOoptional: ad image placeholder takes up dimensions, so no "popping" and content shifting when it finally loads (shit annoys the FUCK out of me, but eh almost every desktop environment is guilty of it as well (highly considering changing to one of those tile based ones... (more likely to code one myself xD (but eh implementing freedesktop protocols sounds cumbersome))))
    //TODOoptional: when "no web view", show list of desktop apps <--> extensions mapping
    //TODOoptional: "live stream", link to p2p vidya instructions. optional because i want to have http streaming once i can afford it. but even then, p2p could be higher quality. don't implement yet because p2p video solutions all look shit imo (perhaps code one ;-P)...
    //TODOoptional: browse to video from dir view, hit back, video (AUDIO!) still playing at dir view
    //TODOoptional: hitting "back" after clicking "random point in time" brings you to yet another random point in time, not the "view my brain" page that the "random point in time" link lives on
    //TODOoptional: "qqq" doesn't appear to work, at least not after a single "q" was requested (maybe OT: the sftp connection was down the whole time)
    //TODOoptimization: thumbnails for pictures (click or dl buttan = full)
    //TODOoptional: allow adding/removing video propagation neighbor[s?] without stopping/restarting server (editting settings in between)

    //random: mfw "moving a file to overwrite another file" lets processes reading the old version continue reading. it's liek free atomicity! could/should have used that for vidya segment lookin up (a fucking "lastSegment" file = gg), am considering using it for .lastModifiedTimestamps updating... except shit when do the readers close it then? when the session ends? a bunch of sessions never ending = old copies stay around forever (not that it should matter to me, so long as they're just using hdd and not memory (err that their memory can be used for other stuffz at times i guess idk what i'm on about (but let's just say i'm glad i haven't coded anything using that yet (i also wonder if it's portable..))))

    WVBoxLayout *menuContentsVLayout = new WVBoxLayout(root());
    menuContentsVLayout->setContentsMargins(0, 0, 0, 0);
    menuContentsVLayout->setSpacing(0);

    WContainerWidget *topContainer = new WContainerWidget();
    topContainer->setContentAlignment(Wt::AlignLeft | Wt::AlignTop);
    topContainer->setHeight(HVBS_ABC2_AD_IMAGE_HEIGHT);
    topContainer->setPadding(0);
    topContainer->setMargin(0);

    WHBoxLayout *topHBoxLayout = new WHBoxLayout(topContainer);
    topHBoxLayout->setContentsMargins(0, 0, 0, 0);
    topHBoxLayout->setSpacing(3);

    //My sexy face logo
    WImage *mySexyFaceLogoImage = new WImage(WLink(WLink::Url, "/my.sexy.face.logo.jpg"), "d3fault");
    //we connect to the clicked signal and do this manually instead because clicking my sexy face doesn't "show the latest video segment" otherwise: WAnchor *homeAnchor = new WAnchor(WLink(WLink::InternalPath, "/"), mySexyFaceLogoImage);
    WAnchor *homeAnchor = new WAnchor(WLink(), mySexyFaceLogoImage);
    homeAnchor->clicked().connect(this, &HackyVideoBullshitSiteGUI::handleHomeAnchorClickedSoShowLatestVideoSegmentEvenIfAlreadyHome);
    homeAnchor->setToolTip("d3fault");
    mySexyFaceLogoImage->setToolTip("d3fault");
    topHBoxLayout->addWidget(homeAnchor);

    //Advertisement
    m_AdImage = new WImage();
    m_AdImage->resize(HVBS_ABC2_AD_IMAGE_WIDTH, HVBS_ABC2_AD_IMAGE_HEIGHT); //TODOreq: share a define with Abc2. also reminds me (though a little off topic) the mime/header of the resource and also the expiration date
    m_AdImage->setHiddenKeepsGeometry(true);
    m_AdImageAnchor = new WAnchor(WLink(), m_AdImage);
    m_AdImageAnchor->setTarget(TargetNewWindow);
    m_AdImageAnchor->setHiddenKeepsGeometry(true);
    m_AdImageAnchor->setPadding(0);
    topHBoxLayout->addWidget(m_AdImageAnchor, 0, Wt::AlignLeft | Wt::AlignTop);

    //Links
    WContainerWidget *linksContainer = new WContainerWidget();
    linksContainer->setContentAlignment(Wt::AlignLeft | Wt::AlignTop);
    topHBoxLayout->addWidget(linksContainer, 1, Wt::AlignLeft | Wt::AlignTop);

    WAnchor *viewMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER), HVBS_VIEW_MYBRAIN_STRING, linksContainer);
    viewMyBrainAnchor->setToolTip(HVBS_VIEW_MYBRAIN_TOOLTIP);
    viewMyBrainAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));
    //viewMyBrainAnchor->decorationStyle().setBorder(WBorder(WBorder::Solid, WBorder::Thin, WColor(255, 255, 255)), Wt::Bottom);
    //browseMyBrainAnchor->setTarget(TargetNewWindow);
    //olo: browseEverythingAnchor->decorationStyle().setTextDecoration(WCssDecorationStyle::Blink);
    new WBreak(linksContainer);
    WAnchor *storeAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_TO_STORE), "Store / Donate", linksContainer);
    storeAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));
    storeAnchor->setToolTip("Buying is basically donating, since everything is already available for free...");
    new WBreak(linksContainer);
    WAnchor *yourAdHereAnchor = new WAnchor(WLink(WLink::Url, HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL), "Your Ad Here", linksContainer);
    yourAdHereAnchor->decorationStyle().setForegroundColor(WColor(255,0,0)); //I like my [necessary] evils to be clearly marked as such
    yourAdHereAnchor->setTarget(TargetNewWindow);

    menuContentsVLayout->addWidget(topContainer);

    m_MainContentsContainer = new WContainerWidget();
    m_MainContentsContainer->setContentAlignment(Wt::AlignLeft | Wt::AlignTop);
    m_MainContentsContainer->setPadding(0);
    m_MainContentsContainer->setMargin(0);
    m_MainContentsContainer->setOverflow(WContainerWidget::OverflowAuto);

    menuContentsVLayout->addWidget(m_MainContentsContainer, 1);

    WPanel *copyrightDropDown = new WPanel(blahRootRedirect());
    copyrightDropDown->setCollapsible(true);
    copyrightDropDown->setCollapsed(true);
    copyrightDropDown->setTitle("Copyright (C) 2014 Steven Curtis Wieler II <http://d3fault.net/> -- All Freedoms Preserved -- Click here for more information about your right to copy");
    WText *copyrightText = new WText(m_CopyrightText, Wt::XHTMLUnsafeText);
    copyrightText->decorationStyle().setBackgroundColor(WColor(0,0,0));
    copyrightText->decorationStyle().setForegroundColor(WColor(255,255,255));
    copyrightText->decorationStyle().setBorder(WBorder(WBorder::Solid, WBorder::Thin, WColor(104, 128, 160)));
    copyrightDropDown->setCentralWidget(copyrightText);
    //copyrightDropDown->decorationStyle().setCursor(PointingHandCursor);

    WPanel *dplDropDown = new WPanel(blahRootRedirect());
    dplDropDown->setCollapsible(true);
    dplDropDown->setCollapsed(true);
    dplDropDown->setTitle("d3fault public license, version 3+ -- Click here to read the legal text");
    WText *dplText = new WText(m_DplLicenseText, Wt::XHTMLUnsafeText);
    dplText->decorationStyle().setBackgroundColor(WColor(0,0,0));
    dplText->decorationStyle().setForegroundColor(WColor(255,255,255));
    dplText->decorationStyle().setBorder(WBorder(WBorder::Solid, WBorder::Thin, WColor(104, 128, 160)));
    dplDropDown->setCentralWidget(dplText);
    //dplDropDown->decorationStyle().setCursor(PointingHandCursor);

    //new WBreak(blahRootRedirect()); eh weird indentation without this WBreak, BUT i'll take that over an entire wasted line!
    WAnchor *bitcoinFaqAnchor = new WAnchor(WLink(WLink::Url, "https://bitcoin.org/en/faq"), "Bitcoin", blahRootRedirect());
    bitcoinFaqAnchor->setTarget(TargetNewWindow);
    new WText(" donate address: 1FwZENuqEHHNCAz4fiWbJWSknV4BhWLuYm", Wt::XHTMLUnsafeText, blahRootRedirect());

    new WBreak(blahRootRedirect());
    new WBreak(blahRootRedirect());

    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred) //design-wise, the setting of this to true should be inside the body of this if. fuck it
    {
        deferRendering();
    }
    else
    {
        enableUpdates(true); //TODOoptimization: if we know that we just got updated with an ACTUAL ad, we know it won't expire for ~24 hours, so we can disable and re-enable this (WTimer driven) after like 23 hours, 50 minutes (or so, idfk). the target (23:50) would also need "5 minute spanning", so perhaps 23:45-23:50
    }

    internalPathChanged().connect(this, &HackyVideoBullshitSiteGUI::handleInternalPathChanged);
    handleInternalPathChanged(internalPath());

    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred || env.agentIsGecko() /* iceweasel would do the animation only a few times and then not show it if clicked again. maybe newer versions of firefox have this fixed, but iceweasel 17.0.10 is bugged */)
        return;

    WAnimation slideDownAnimation(WAnimation::SlideInFromTop, WAnimation::EaseOut, 250);

    copyrightDropDown->setAnimation(slideDownAnimation);
    dplDropDown->setAnimation(slideDownAnimation);
}
void HackyVideoBullshitSiteGUI::finalize()
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "unsubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, this));
}
void HackyVideoBullshitSiteGUI::handleInternalPathChanged(const string &newInternalPath)
{
    //TODOmb: remove trailing slash when considering what to display, so /MyBrain/ matches /MyBrain, etc

    //layout defaults (m_Contents pointing to zero)
    if(m_Contents)
    {
        delete m_Contents;
        m_Contents = 0; //this isn't used [anymore] to "add the content into/onto", but is now just a pointer to the content added DIRECTLY to root(). reasoning: WText word wrapping doesn't work when it's a child of WContainerWidget, but i still need a way to delete it (hence, pointer)
    }
    //m_Contents = new WContainerWidget(root());

    if(newInternalPath == "/" || newInternalPath == "" || newInternalPath == HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Latest")
    {
        deleteTimelineAndDirectoryBrowsingStackIfNeeded();

        WEnvironment::CookieMap::const_iterator maybeTimestampOfVideoLastWatched = environment().cookies().find(HVBS_TIMESTAMP_OF_VIDEO_LAST_WATCHED_COOKIE_KEY);
        if(maybeTimestampOfVideoLastWatched != environment().cookies().end())
        {
            //HAVE COOKIE

            std::string unsanitizedTimestampOfVideoLastWatched = maybeTimestampOfVideoLastWatched->second;
            const std::string &latestVideoSegmentPath = determineLatestVideoSegmentPathOrUsePlaceholder();
            if(latestVideoSegmentPath == HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER)
            {
                displayVideoSegment(HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER);
                return;
            }
            bool convertOk = false;
            qint64 maybeAnumberTimestampOfVideoLastWatched = QString::fromStdString(unsanitizedTimestampOfVideoLastWatched).toLongLong(&convertOk);
            //WIntValidator timestampSanitizer(0, INT_MAX);
            //timestampSanitizer.setMandatory(true);
            //if(timestampSanitizer.validate(unsanitizedTimestampOfVideoLastWatched).state() == WIntValidator::Valid)
            if(convertOk)
            {
                //timestamp is a number, but that doesn't mean it exists, or that a vid exists after it, etc
                qint64 latestVideoSegmentSecondsSinceEpoch = determineSecondsSinceEpochFromVideoSegmentFilepath(latestVideoSegmentPath); //for use as sanity "upper limit"
                if(maybeAnumberTimestampOfVideoLastWatched >= latestVideoSegmentSecondsSinceEpoch)
                {
                    //either their timestamp was bullshit (too high), or it is pointing at the latest video segment
                    displayVideoSegment(latestVideoSegmentPath);
                    return;
                }

                //getting here, we know there is AT LEAST one video after the timestamp they provided (worst case, it is the latest video segment)

                //find the first vid after that timestamp that exists -- hmm this code is basically identical to "play vid roughly 24 hours ago", except with a dynamic timestamp (*merges*)
                displayVideoSegment(determineFirstVideoSegmentThatComesChronologicallyAfterThisEpochTimeInSeconds(maybeAnumberTimestampOfVideoLastWatched, latestVideoSegmentSecondsSinceEpoch));
                return;
            }
            else
            {
                displayVideoSegment(latestVideoSegmentPath);
                return;
            }
        }
        else
        {
            //NO COOKIE

            displayEither_LatestVideoSegmentIfLive_or_24hrOldVideoIfNotLive(); //TODOreq: "latest vid" button. TODOoptional: "resume where I left of OR show latest if live (use 4 min test)" checkbox... it's a bit much/confusing, but is also handy af. or maybe a popup "want to resume where you left off, or watch the live stream?"
        }
        return;
    }

    if(newInternalPath == HVBS_WEB_CLEAN_URL_HACK_TO_VIEW_MYBRAIN_ON_PLATTER) //Note: a file/dir called "MyBrain" will never be viewed xD, perhaps i should make it and explain in it
    {
        deleteTimelineAndDirectoryBrowsingStackIfNeeded();

        WContainerWidget *viewMyBrainContainerWidget = new WContainerWidget();

        new WText("There are 3 ways to view my [archived] brain, take your pick:",  viewMyBrainContainerWidget);

        new WBreak(viewMyBrainContainerWidget);
        new WBreak(viewMyBrainContainerWidget);

        new WText("1: ", Wt::XHTMLUnsafeText, viewMyBrainContainerWidget);
        WAnchor *myBrainTimelineAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_TIMELINE), HVBS_TIMELINE_MY_BRAIN_STRING, viewMyBrainContainerWidget);
        myBrainTimelineAnchor->setToolTip(HVBS_TIMELINE_MYBRAIN_TOOLTIP);

        new WBreak(viewMyBrainContainerWidget);

        new WText("2: ", Wt::XHTMLUnsafeText, viewMyBrainContainerWidget);
        WAnchor *browseMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN), HVBS_BROWSE_MY_BRAIN_STRING, viewMyBrainContainerWidget);
        browseMyBrainAnchor->setToolTip(HVBS_BROWSE_MYBRAIN_TOOLTIP);

        new WBreak(viewMyBrainContainerWidget);

        new WText("3: ", Wt::XHTMLUnsafeText, viewMyBrainContainerWidget);
        //TODOreq: Download In Full (link to torrents n increments and shit, could even just be a directory to simplify adding increments...)
        WAnchor *downloadMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_DOWNLOAD), HVBS_DOWNLOAD_MY_BRAIN_STRING, viewMyBrainContainerWidget);
        downloadMyBrainAnchor->setToolTip(HVBS_DOWNLOAD_MYBRAIN_TOOLTIP);

        setMainContent(viewMyBrainContainerWidget);
        return;
    }

    if(newInternalPath == HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_DOWNLOAD)
    {
        deleteTimelineAndDirectoryBrowsingStackIfNeeded();

        //TODOreq: host and link to the torrent files (tpb is dead also so this needs to be done soon). Maybe just a magnet URL instead?
        WContainerWidget *downloadContainer = new WContainerWidget();

        new WText("Password: L^_t^Cw!X)gvWD0;*;d*n-R+|nOjA1dO`CO^sZ9$zcN=kLL(zG9xHKP}bQQwH+~", downloadContainer);

        new WBreak(downloadContainer);
        new WBreak(downloadContainer);

        new WText("Download in full:", Wt::XHTMLUnsafeText, downloadContainer);
        new WBreak(downloadContainer);
        WAnchor *tpbMyBrainPublicFilesAnchor = new WAnchor(WLink(WLink::Url, HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_PUBLIC_RESOURCE), "My Brain - Public Files", downloadContainer);
        tpbMyBrainPublicFilesAnchor->setTarget(TargetNewWindow);

        new WBreak(downloadContainer);
        new WBreak(downloadContainer);

        new WText("2014 Supplement:", Wt::XHTMLUnsafeText, downloadContainer);
        new WBreak(downloadContainer);
        WAnchor *tpbMyBrain2014supplementAnchor = new WAnchor(WLink(WLink::Url, HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_PUBLIC_RESOURCE), "My Brain - 2014 Supplement", downloadContainer);
        tpbMyBrain2014supplementAnchor->setTarget(TargetNewWindow);


        new WBreak(downloadContainer);
        new WBreak(downloadContainer);

        new WText("Hold onto this for me plz:", Wt::XHTMLUnsafeText, downloadContainer);
        new WBreak(downloadContainer);
        WAnchor *tpbMyBrainPrivateFilesAnchor = new WAnchor(WLink(WLink::Url, HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_PUBLIC_RESOURCE), "My Brain - Private Files", downloadContainer);
        tpbMyBrainPrivateFilesAnchor->setTarget(TargetNewWindow);

        new WBreak(downloadContainer);
        new WBreak(downloadContainer);
        new WBreak(downloadContainer);
        new WBreak(downloadContainer);

        new WText("Github (text only): git clone https://github.com/d3faultdotxbe/autobackup.git", downloadContainer);
        new WBreak(downloadContainer);
        new WText("Pull requests welcome/encouraged. Derive from me plx!", downloadContainer); //TODOreq: Gerrit

        setMainContent(downloadContainer);
        return;
    }

    if(newInternalPath == "/licenses" || newInternalPath == "/licenses/")
    {
        deleteTimelineAndDirectoryBrowsingStackIfNeeded();

        WContainerWidget *licensesContainer = new WContainerWidget();
        new WText("Current version of d3fault public license: 3", licensesContainer);
        new WBreak(licensesContainer);
        new WText("Link: ", licensesContainer);
        new WAnchor(WLink(WLink::InternalPath, "/autobackupLatest/license.dpl.txt"), "d3fault public license - version 3", licensesContainer);

        setMainContent(licensesContainer);
        return;
    }

    if(newInternalPath == HVBS_WEB_CLEAN_URL_TO_STORE)
    {
        deleteTimelineAndDirectoryBrowsingStackIfNeeded();

        WContainerWidget *storeContainer = new WContainerWidget();
        new WText("<b>Book:</b>", storeContainer);

        new WBreak(storeContainer);

        new WText("Everything: ", storeContainer);
        WAnchor *bookPart1of5Anchor = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/4973161"), "Part 1/5", storeContainer);
        bookPart1of5Anchor->setTarget(TargetNewWindow);
        new WText(" - ", storeContainer);
        WAnchor *bookPart2of5Anchor = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/5329954"), "Part 2/5", storeContainer);
        bookPart2of5Anchor->setTarget(TargetNewWindow);
        new WText(" - ", storeContainer);
        WAnchor *bookPart3of5Anchor = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/5329957"), "Part 3/5", storeContainer);
        bookPart3of5Anchor->setTarget(TargetNewWindow);
        new WText(" - ", storeContainer);
        WAnchor *bookPart4of5Anchor = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/5329960"), "Part 4/5", storeContainer);
        bookPart4of5Anchor->setTarget(TargetNewWindow);
        new WText(" - ", storeContainer);
        WAnchor *bookPart5of5Anchor = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/5329974"), "Part 5/5", storeContainer);
        bookPart5of5Anchor->setTarget(TargetNewWindow);

        new WBreak(storeContainer);

        new WText("A lot less: ", storeContainer);
        WAnchor *bookDreamsExcerpts = new WAnchor(WLink(WLink::Url, "https://www.createspace.com/5330000"), "Dreams Excerpts", storeContainer);
        bookDreamsExcerpts->setToolTip("d3fault's dream exceprts, for those who don't care to read code or ramblings.");
        bookDreamsExcerpts->setTarget(TargetNewWindow);

        setMainContent(storeContainer);
        return;
    }

    std::string rewrittenInternalPath = newInternalPath;
    if(newInternalPath == HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN)
    {
        rewrittenInternalPath = ""; //hack to be able to browse the root directory, navigable via /MyBrain/Browse
    }

    QString theInternalPathCleanedQString = QDir::cleanPath(QString::fromStdString(rewrittenInternalPath)); //strips trailing slash if dir
    if(theInternalPathCleanedQString.contains("/../", Qt::CaseSensitive) || theInternalPathCleanedQString == "/..")
    {
        hvbs404();
        //quit();
        return;
    }

    if(!m_TimelineAndDirectoryLogicalContainer)
    {
        m_TimelineAndDirectoryLogicalContainer = new WContainerWidget(blahRootRedirect());
        m_TimelineAndDirectoryLogicalContainer->setInline(true);

        new WText("You are here: ", Wt::XHTMLUnsafeText, m_TimelineAndDirectoryLogicalContainer);
        new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN), "d3fault.net", m_TimelineAndDirectoryLogicalContainer); //TODOoptional: customizeable domain
        m_CookieCrumbContainerWidget = new WContainerWidget(m_TimelineAndDirectoryLogicalContainer);
        m_CookieCrumbContainerWidget->setInline(true);
        m_CookieCrumbContainerWidget->setContentAlignment(Wt::AlignMiddle);
        new WBreak(m_TimelineAndDirectoryLogicalContainer);

        m_TimelineAndDirectoryBrowsingStack = new WStackedWidget(m_TimelineAndDirectoryLogicalContainer);
    }

    if(theInternalPathCleanedQString == HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_TIMELINE) //not specifying = random point in time <3 (couldn't decide whether to start with earliest or latest... so... i didn't)
    {
        newTimelineIfNeededAndBringToFront();
        m_TimeLineMyBrainWidget->redirectToRandomPointInTimeline();
        return;
    }

    //NVM: OT'ish: i'm now trying to change this shit so it can deal with multiple dirs/.lastModifiedTimestamps (just 3) ... and I think this is coincidentally future proofing me for git submdules shits :-P (hundreds/thousands). maybe not.

    const QString &myBrainItemToPresentAbsolutePathQString = m_HvbsWebBaseDirActual_NOT_CLEAN_URL_NoSlashAppended + "/view" + theInternalPathCleanedQString;
    if(QFile::exists(myBrainItemToPresentAbsolutePathQString))
    {
        QFileInfo myBrainItemFileInfo(myBrainItemToPresentAbsolutePathQString);
        if(myBrainItemFileInfo.isFile())
        {
            createCookieCrumbsFromPath(/*relativePath_aka_internalPath*/);
            newTimelineIfNeededAndBringToFront();
            m_TimeLineMyBrainWidget->presentFile(theInternalPathCleanedQString, myBrainItemToPresentAbsolutePathQString, myBrainItemFileInfo.fileName().toStdString());
            return;
        }
        else if(myBrainItemFileInfo.isDir())
        {
            createCookieCrumbsFromPath(/*relativePath_aka_internalPath*/);
            if(!m_BrowseMyBrainDirWidget)
            {
#if 0
                const std::string *pageNumParam = environment().getParameter("page");
                int pageNumToUse = 1;
                if(pageNumParam)
                {
                    bool convertOk = false;
                    QString numQString = QString::fromStdString(*pageNumParam);
                    int attempt = numQString.toInt(&convertOk);
                    if(convertOk)
                        pageNumToUse = attempt;
                }
#endif
                //m_BrowseMyBrainDirWidget = new DirectoryBrowsingWtWidget(myBrainItemToPresentAbsolutePathQString, theInternalPathCleanedStdString,/*pageNumToUse,*/ m_TimelineAndDirectoryBrowsingStack);
                m_BrowseMyBrainDirWidget = new DirectoryBrowsingWtWidget(m_TimelineAndDirectoryBrowsingStack);
                m_TimelineAndDirectoryBrowsingStack->setCurrentWidget(m_BrowseMyBrainDirWidget);
                m_BrowseMyBrainDirWidget->showDirectoryContents(myBrainItemToPresentAbsolutePathQString, theInternalPathCleanedQString);
                return;
            }

            if(m_TimelineAndDirectoryBrowsingStack->currentWidget() != m_BrowseMyBrainDirWidget)
            {
                m_TimelineAndDirectoryBrowsingStack->setCurrentWidget(m_BrowseMyBrainDirWidget);
            }
            m_BrowseMyBrainDirWidget->showDirectoryContents(myBrainItemToPresentAbsolutePathQString, theInternalPathCleanedQString); //TODOreq: browser button "back" stays on page it was on

#if 0 //OLD: pre-qdiriterator && paginization
            QDir myBrainFolder(myBrainItemToPresentAbsolutePathQString);
            const QStringList myBrainFolderEntryList = myBrainFolder.entryList((QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden), (QDir::Time /* | QDir::Reversed*/)); //OOPS BACKWARDS: sorting by reverse time shows my latest (better) stuff at the top (people don't scroll), sorting normal shows my oldest (lol) stuff at the top. one flag will determine whether or not i make enough in ad revenue to not have to commit suicide
            int myBrainFolderEntryListSize = myBrainFolderEntryList.size();
            for(int i = 0; i < myBrainFolderEntryListSize; ++i)
            {
                //TODOoptional: sort dirs at top and make them visually different
                const std::string &currentEntryString = myBrainFolderEntryList.at(i).toStdString();
                new WAnchor(WLink(WLink::InternalPath, theInternalPathCleanedStdString + "/" + currentEntryString), currentEntryString, directoryBrowsingContainerWidget);
                new WBreak(directoryBrowsingContainerWidget);
            }
#endif
            return;
        }
    }
    hvbs404(); //Note (had:TODOforever): if adding more non-404 cases, they should end with "return;" just like the others do...
}
//TODOoptional: serialized wt instructions/etc passed in here and i could grow/transform the entire site before their eyes (has tons of WOO factor (but tbh, 'the web' (http/www) is fucking boring/shit), with the video being the only constant factor. i don't know the 'best' way to do what i describe, but i could do it hackily and i'm sure there's a 'proper' way to do it as well (would make for interesting reading). I've thought more on this just because it's mildly interesting, and I think dynamic/run-time plugins would be the best way to do it
void HackyVideoBullshitSiteGUI::handleAdImageChanged(WResource *newAdImageResource, string newAdUrl, string newAdAltAndHover)
{
    if(m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred)
    {
        resumeRendering();
        m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred = false;
    }

    //set the ad that the backend gave us

    if(newAdUrl == "n") //hack. both newAdImageResource and newAdAltAndHover are undefined if url is "n" (they may be 0/empty, or they may still be 'yesterdays')
    {
        //set up "no ad" placeholder
        m_AdImage->setImageLink(WLink(WLink::Url, /*http://d3fault.net*/"/no.ad.placeholder.jpg"));
        m_AdImage->setAlternateText(HVBS_ABC2_PLACEHOLDER_ALT_AND_HOVER);
        m_AdImage->setToolTip(HVBS_ABC2_PLACEHOLDER_ALT_AND_HOVER);
        //already set as optimization and to neutralize popping: m_AdImage->resize(HVBS_ABC2_AD_IMAGE_WIDTH, HVBS_ABC2_AD_IMAGE_HEIGHT);

        m_AdImageAnchor->setLink(WLink(WLink::Url, HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL));
        m_AdImageAnchor->setToolTip(HVBS_ABC2_PLACEHOLDER_ALT_AND_HOVER);
        //already set, would be undefined i'd imagine: m_AdImageAnchor->setImage(m_AdImage);

        if(!environment().ajax())
        {
            return;
        }
        triggerUpdate();
        return;
    }

    //TODOoptional: it would be trivial to show a 'price countdown' just like on abc2 just below the image itself (and has a wow factor of over 9000), but actually UPDATING that value on new purchases would NOT be [as] trivial :-/... so fuck it

    m_AdImage->setImageLink(WLink(newAdImageResource));
    m_AdImage->setAlternateText(newAdAltAndHover);
    m_AdImage->setToolTip(newAdAltAndHover);

    m_AdImageAnchor->setLink(WLink(WLink::Url, newAdUrl));
    m_AdImageAnchor->setToolTip(newAdAltAndHover);

    if(!environment().ajax())
    {
        //no-js will see the ad image update if/when they refresh or click a link to another item (TODOreq: test/confirm this, because i'm not sure it's true). COULD use a timer (which i'm more sure will work), but eh... naw..
        //^if you actually do any code for this, should probably copy/paste it to the no ad placeholder part too
        return;
    }
    triggerUpdate();
}
void HackyVideoBullshitSiteGUI::displayEither_LatestVideoSegmentIfLive_or_24hrOldVideoIfNotLive()
{
    const std::string &latestVideoSegmentPath = determineLatestVideoSegmentPathOrUsePlaceholder();
    if(latestVideoSegmentPath == HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER)
    {
        displayVideoSegment(HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER);
        return;
    }
    qint64 latestVideoSegmentSecondsSinceEpoch = determineSecondsSinceEpochFromVideoSegmentFilepath(latestVideoSegmentPath);
    if(latestVidIsLessThan4minutesOld(latestVideoSegmentSecondsSinceEpoch))
    {
        //[probably] live, so display latest video
        displayVideoSegment(latestVideoSegmentPath);
    }
    else
    {
        //[probably not] live, so display ~24hours (err towards newer) old vid
        displayVideoSegmentRoughly24hoursBefore(latestVideoSegmentSecondsSinceEpoch);
    }
}
void HackyVideoBullshitSiteGUI::displayVideoSegment(const string &videoSegmentFilePath)
{
    WContainerWidget *videoSegmentContainer = new WContainerWidget();

    WPushButton *videoSegmentDownloadButton = new WPushButton(HVBS_DOWNLOAD_LOVE, videoSegmentContainer);
    //TODOreq: "Link to this: ", needs to chop off base dir from absolute path to make clean url...
    new WBreak(videoSegmentContainer);

    //TODOreq: previous button
    WPushButton *nextVideoClipPushButton = new WPushButton("Next Clip", videoSegmentContainer); //if next != current; aka if new-current != current-when-started-playing
    nextVideoClipPushButton->clicked().connect(this, &HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked);
    if(environment().ajax())
    {
        WCheckBox *autoPlayNextVideoSegmentCheckbox = new WCheckBox("Auto-Play Next ", videoSegmentContainer);
        autoPlayNextVideoSegmentCheckbox->setChecked(m_AutoPlayNextVideoSegmentCheckboxChecked);
        autoPlayNextVideoSegmentCheckbox->checked().connect(this, &HackyVideoBullshitSiteGUI::handleAutoPlayNextVideoSegmentCheckboxChecked);
        autoPlayNextVideoSegmentCheckbox->unChecked().connect(this, &HackyVideoBullshitSiteGUI::handleAutoPlayNextVideoSegmentCheckboxUnchecked);
    }
    else
    {
        new WText(" ", videoSegmentContainer);
    }
    //TODOreq: latest  button (perhaps disabled at first, noop if latest is what we're already at)
    new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS), "Browse All Video Clips", videoSegmentContainer);

    new WText(" ", videoSegmentContainer);

    WAnchor *randomFileAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_TIMELINE), "Random File", videoSegmentContainer);
    randomFileAnchor->decorationStyle().setForegroundColor(WColor(0,255,0)); //bitches need visual cues because bitches is dumb as fuck n shit

    new WBreak(videoSegmentContainer);

    if(videoSegmentFilePath != HVBS_VIDEO_SEGMENT_SPECIAL_PATH_TO_INDICATE_CURRENT_IS_LATEST)
    {
        m_VideoSegmentFilePathCurrentlyBeingDisplayed = videoSegmentFilePath;
        WVideo *videoSegmentPlayer = new WVideo(videoSegmentContainer);
        if(environment().ajax())
        {
            videoSegmentPlayer->ended().connect(this, &HackyVideoBullshitSiteGUI::handleLatestVideoSegmentEnded);
        }
        videoSegmentPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
        videoSegmentPlayer->resize(720, 480);
        //videoSegmentPlayer->resize(800, 600);
        videoSegmentPlayer->setAlternativeContent(new WText(HVBS_NO_HTML5_VIDEO_OR_ERROR, Wt::XHTMLUnsafeText));


        WResource *videoSegmentResource = new WFileResource("video/ogg", videoSegmentFilePath, videoSegmentPlayer);
        QFileInfo fileInfo(QString::fromStdString(videoSegmentFilePath));
        const std::string &filenameOnly = fileInfo.fileName().toStdString();
        videoSegmentResource->suggestFileName(filenameOnly, WResource::Attachment);
        videoSegmentPlayer->addSource(WLink(videoSegmentResource), "video/ogg");
        videoSegmentDownloadButton->setResource(videoSegmentResource);

        qint64 timestampOVideoLastWatched = determineSecondsSinceEpochFromVideoSegmentFilepath(videoSegmentFilePath);
        QString timestampOVideoLastWatchedQString = QString::number(timestampOVideoLastWatched);
        const std::string &timestampOVideoLastWatchedStdString = timestampOVideoLastWatchedQString.toStdString();
        WDateTime expireDateTime = WDateTime::currentDateTime().addYears(1);
        setCookie(HVBS_TIMESTAMP_OF_VIDEO_LAST_WATCHED_COOKIE_KEY, timestampOVideoLastWatchedStdString, expireDateTime);
    }
    else
    {
        new WText("Sorry, the video you just watched is the latest one available. The next video will likely be uploaded within a few minutes or years, so try again sometime in between.", videoSegmentContainer);
        new WBreak(videoSegmentContainer);
        WPushButton *tryAgainButton = new WPushButton("Try Again", videoSegmentContainer);
        tryAgainButton->clicked().connect(this, &HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked);
    }

    setMainContent(videoSegmentContainer);
}
void HackyVideoBullshitSiteGUI::handleAutoPlayNextVideoSegmentCheckboxChecked()
{
    m_AutoPlayNextVideoSegmentCheckboxChecked = true;
}
void HackyVideoBullshitSiteGUI::handleAutoPlayNextVideoSegmentCheckboxUnchecked()
{
    m_AutoPlayNextVideoSegmentCheckboxChecked = false;
}
void HackyVideoBullshitSiteGUI::handleLatestVideoSegmentEnded()
{
    if(m_AutoPlayNextVideoSegmentCheckboxChecked)
    {
        //TODOoptional: when it's checked, generate a random number so that there's a 1% (etc) chance we set it to unchecked and disable the auto-play next functionality. Eh a sort of bandwidth helper in case people walk away from comp... but mostly just troll functionality <3
        handleNextVideoClipButtonClicked();
    }
}
//assumes there is at least one entry in the folder! will segfault otherwise xD!!
QString HackyVideoBullshitSiteGUI::getEarliestEntryInFolder(const QString &folderWithSlashAppended)
{
    QDir folderDir(folderWithSlashAppended);
    const QStringList folderEntries = folderDir.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Time)); //we know this isn't empty because we move the folder into place with a file already in it
    const QString &earliestEntryInFolder = folderEntries.last(); //last is 'first' (chronologically), since we sort reversed xD
    QString ret = folderWithSlashAppended + earliestEntryInFolder;
    return ret;
}
void HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked()
{
    /*
     day/year (code can be reused with airbourne timeline) vs. live stream (with segments ONLY for archives)
    I'm considering refactoring this code to account for day/year GAPS (make it proper), but then idk I think it might be a waste of time if I decide to use a live stream on the front page (instead of 3 minute segments ONLY)

    segemnt pro, live con: disconnect/reconnect of my upload connection does not ever 'drop' seconds/minutes (for segments, it would make users be behind 'live (near live)' by however long the downtime was (but they don't miss a second of video)... with live they always catch up to live (even if that means they miss some seconds)

    decisions decisions
    */

    if(m_Contents) //TODOoptimization: don't delete/new the video container/player/download button etc (it's very likely that m_Contents points to the video wcontainer widget created in displayVideoSegment(). right now i need to for safety/KISS reasons
    {
        delete m_Contents;
        m_Contents = 0;
    }
    if(m_VideoSegmentFilePathCurrentlyBeingDisplayed == "" || m_VideoSegmentFilePathCurrentlyBeingDisplayed == HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER)
    {
        displayVideoSegment(determineLatestVideoSegmentPathOrUsePlaceholder());
        return;
    }

    //figure out next video clip based on current video clip -- if there is no "next", i should... TODOreq?? maybe just say so, and maybe if the auto-play next button is checked, try again in like 10 mins? idfk
    QString videoSegmentFilePathCurrentlyBeingDisplayedQString = QString::fromStdString(m_VideoSegmentFilePathCurrentlyBeingDisplayed);
    QFileInfo fileInfo(videoSegmentFilePathCurrentlyBeingDisplayedQString);
    const QString &currentVideoSegmentFilenameOnly = fileInfo.fileName();
    const QString &currentVideoSegmentFilenameMinusExt = currentVideoSegmentFilenameOnly.left(currentVideoSegmentFilenameOnly.lastIndexOf("."));
    bool convertOk = false;
    long long currentVideoSegmentFilenameMinusExtAsLong = currentVideoSegmentFilenameMinusExt.toLongLong(&convertOk);
    if(!convertOk)
    {
        displayVideoSegment(HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER);
        return;
    }
    const QDateTime &currentVideoSegmentFilenamesDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(currentVideoSegmentFilenameMinusExtAsLong)*1000).toUTC();
    const int currentVideoSegmentFilenamesYear = currentVideoSegmentFilenamesDateTime.date().year();
    const int currentVideoSegmentFilenamesDayOfYear = currentVideoSegmentFilenamesDateTime.date().dayOfYear();

    //we know this year/day folder exists because we just played a video from it. we also know it isn't empty for the same reason
    const QString &currentVideoSegmentYearFolder_AbsoluteAndWithSlashAppended = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + QString::number(currentVideoSegmentFilenamesYear) + QDir::separator();
    const QString &currentVideoSegmentDayFolder = currentVideoSegmentYearFolder_AbsoluteAndWithSlashAppended + QString::number(currentVideoSegmentFilenamesDayOfYear) + QDir::separator();

    QDir currentVideoSegmentDayDir(currentVideoSegmentDayFolder);
    const QStringList all3MinuteSegmentsInDayFolder = currentVideoSegmentDayDir.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Time));

    if(all3MinuteSegmentsInDayFolder.first() == currentVideoSegmentFilenameOnly)
    {
        //possible day shift, or possibly current is LATEST. might be entry point for possible year shifts as well
        //TO DOnereq: seeing if "tomorrow" is the "366th" day of the year is not a good way to detect year shifts, because of leap years xD
        const QDateTime &dayAfterCurrentVideoSegment = currentVideoSegmentFilenamesDateTime.addDays(1); //doc says this adjusts for daylight savings, so I'm assuming it adjusts for leap years too xD. In Qt we Trust~
        const int maybeYearAfterCurrentVideoSegment = dayAfterCurrentVideoSegment.date().year();
        if(maybeYearAfterCurrentVideoSegment != currentVideoSegmentFilenamesYear)
        {
            //year shift, or possibly current is LATEST

            //Get the next year folder that exists, we may have been offline for 1+ years
            QDir rootVideoSegmentsDir(m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended);
            const QStringList allYearFolders = rootVideoSegmentsDir.entryList((QDir::NoDotAndDotDot | QDir::Dirs), (QDir::Time));
            QString currentVideoSegmentYearString = QString::number(currentVideoSegmentFilenamesYear);
            if(allYearFolders.first() == currentVideoSegmentYearString)
            {
                //CURRENT IS LATEST
                //tellUserThatCurrentVideoSegmentIsLatest();
                displayVideoSegment(HVBS_VIDEO_SEGMENT_SPECIAL_PATH_TO_INDICATE_CURRENT_IS_LATEST);
                return;
            }

            //Now get index into that list of the year that just ended, and subtract one from it... giving us the next year that a video was published
            const int indexOfNextYearAtLeastOneVideoWasPublished = (allYearFolders.indexOf(currentVideoSegmentYearString) - 1); //Since it wasn't first, we know we can safely subtract 1 from that index
            const QString &aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt = allYearFolders.at(indexOfNextYearAtLeastOneVideoWasPublished);
            //now we just get it's entries (day folders), get the last() (which is first chronologically), and do the same inside there except for files
            QString aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt_AbsoluteAndWithSlashAppended = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt + QDir::separator();
            const std::string &videoSegmentFilePathFromFirstDayOfNextYear = getFilePathOfFirstVideoSegmentInFirstDayOfYearFolder(aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt_AbsoluteAndWithSlashAppended);
            displayVideoSegment(videoSegmentFilePathFromFirstDayOfNextYear);
            return;
        }
        //we've ruled out year shift. either day shift or current is latest
        QDir currentVideoSegmentYearDir(currentVideoSegmentYearFolder_AbsoluteAndWithSlashAppended);
        const QStringList allDayFoldersInCurrentYear = currentVideoSegmentYearDir.entryList((QDir::NoDotAndDotDot | QDir::Dirs), (QDir::Time));
        QString currentVideoSegmentFilenamesDayOfYearString = QString::number(currentVideoSegmentFilenamesDayOfYear);
        if(allDayFoldersInCurrentYear.first() == currentVideoSegmentFilenamesDayOfYearString)
        {
            //CURRENT IS LATEST
            //tellUserThatCurrentVideoSegmentIsLatest();
            displayVideoSegment(HVBS_VIDEO_SEGMENT_SPECIAL_PATH_TO_INDICATE_CURRENT_IS_LATEST);
            return;
        }
        int indexOfNextExistingDayFolder = (allDayFoldersInCurrentYear.indexOf(currentVideoSegmentFilenamesDayOfYearString)-1); //safe because current isn't 'first()'
        const QString &nextExistingDayFolder = allDayFoldersInCurrentYear.at(indexOfNextExistingDayFolder);
        //now just get the 'last()' video segment in that day folder (which is the first chronologically)
        QString nextExistingDayFolder_AbsoluteAndWithSlashAppended = currentVideoSegmentYearFolder_AbsoluteAndWithSlashAppended + nextExistingDayFolder + QDir::separator();
        displayVideoSegment(getFilePathOfFirstVideoSegmentInDayFolder(nextExistingDayFolder_AbsoluteAndWithSlashAppended));
        return;
    }

    //since current wasn't 'first' (sorting in reverse), we know that there are videos 'after' it (cronologically)... so we simply find the index of current and subtract 1 in order to get the 'next' video
    const QString &nextVideoFilenameOnly = all3MinuteSegmentsInDayFolder.at(all3MinuteSegmentsInDayFolder.indexOf(currentVideoSegmentFilenameOnly) - 1);
    const QString &nextVideoFilePath = currentVideoSegmentDayFolder + nextVideoFilenameOnly;
    const std::string &nextVideoFilePathStdString = nextVideoFilePath.toStdString();
    displayVideoSegment(nextVideoFilePathStdString);
}
//returns either path to most recent or to placeholder video segment
string HackyVideoBullshitSiteGUI::determineLatestVideoSegmentPathOrUsePlaceholder()
{
#if 0 //OTHER_SOLUTION_TO_SORTING_PROBLEM_MENTIONED_IN_COMMENTS_FOUND_LOL_WOOT
    {
        QDir videoSegmentsRootDir("/run/shm/butts");
        QStringList yearsDirListing = QDir::entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Time); //TODOoptimization: even though the disk buffer cache saves us a hdd read, i'm willing to bet that the sorted results aren't cached. the further along in time i go (years wise), and also the further along int the year i get, and also the further along in the day i get, the more memory and cpu usage needed to re-sort over and over and over. so yea uhh solve that problem (unless it isn't a problem!). but for now KISS :-P
        QDir latestVideoSegmentYearDir(videoSegmentsRootDir.absolutePath() + QDir::separator() + yearsDirListing.first()); //it is definitely an optimization to just use QDateTime to find the year folder, BUT it might not exist if we've transitioned years but not yet moved
    }
#endif
    QDate currentDate = QDateTime::currentDateTimeUtc().date(); //WDate seems more natural given the context, but I doubt it matters.... AND Qt would probably be the faster of the two (or they're identical (this comment not...))
    for(;;) //i think this is faster than while(true) because there's no testing involved, but really i'd bet the compiler optimizes that out, in which case i like the look and readability of while(true) better (forever is just as schmexy (tried using it here but i have no_keywords shit going on guh))
    {
        bool yearFolderFound = QFile::exists(m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + QString::number(currentDate.year())); //woot sorting problem solved for root folder ls SORT'ing
        if(!yearFolderFound)
        {
            currentDate = currentDate.addYears(-1);
            if(currentDate.year() == 2014) //can't time travel...
            {
                return HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER;
            }
            continue; //TODOreq: make sure video segment uploader is started and has uploaded at least one segment (and our VideoSegmentsImporterFolderWatcher has to have imported it and created relevant folders, OTHERWISE this will spinlock until int wraps around from negatives and finally does see the folder existing rofl. will do that for every WApplication too...
        }
        //year folder found, now find day folder using same method
        for(;;)
        {
            const QString &dayFolderToLookFor = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + QString::number(currentDate.year()) + QDir::separator() + QString::number(currentDate.dayOfYear()) + QDir::separator();
            bool dayFolderFound = QFile::exists(dayFolderToLookFor); //TODOreq: as we are subtracting years, would it maybe make dayOfYear change as well in the leap year case (whoever invented leap years should be shot)
            if(!dayFolderFound)
            {
                currentDate = currentDate.addDays(-1);
                continue;
            }
            //day folder found, now find latest segment using sorting :(
            QDir dayFolder(dayFolderToLookFor);
            const QStringList all3MinuteSegmentsInDayFolder = dayFolder.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Time));
            return all3MinuteSegmentsInDayFolder.isEmpty() ? HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER /*perhaps should also prevent the year/day folder spinlock described above */ : (dayFolderToLookFor + all3MinuteSegmentsInDayFolder.first()).toStdString(); //TODOoptimization: read the if'd out stuff above about fixing having to list (cached.) + sort (probably not cached?) the dir. basically ~11:59pm will be more expensive than ~12:01am
        }
    }
    //should never get here, but just in case and to make compiler stfu:
    return HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER;
}
qint64 HackyVideoBullshitSiteGUI::determineSecondsSinceEpochFromVideoSegmentFilepath(const string &videoSegmentFilePath)
{
    QFileInfo latestVideoSegmentFileInfo(QString::fromStdString(videoSegmentFilePath));
    const QString &latestVidTimestampAsString = latestVideoSegmentFileInfo.baseName();
    return latestVidTimestampAsString.toLongLong();
}
bool HackyVideoBullshitSiteGUI::latestVidIsLessThan4minutesOld(qint64 latestVideoSegmentSecondsSinceEpoch)
{
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch() / 1000;
    if((currentTimestamp - latestVideoSegmentSecondsSinceEpoch) <= 4)
    {
        return true;
    }
    return false;
}
void HackyVideoBullshitSiteGUI::displayVideoSegmentRoughly24hoursBefore(qint64 latestVideoSegmentSecondsSinceEpoch) //If I record only 1x 3 minute video segment over the course of 24 hours, it is the one that will be shown xD (this method did nothing). Fuckit
{
    const std::string &videoSegmentRoughly24hoursBeforeLatest = determineFirstVideoSegmentThatComesChronologicallyAfterThisEpochTimeInSeconds(latestVideoSegmentSecondsSinceEpoch - (60*60*24), latestVideoSegmentSecondsSinceEpoch);
    displayVideoSegment(videoSegmentRoughly24hoursBeforeLatest);
}
//this method assumes there is definitely at least one video "after" the target epoch time, and might fail (infinite loop?) otherwise. the caller must ensure that this is true
string HackyVideoBullshitSiteGUI::determineFirstVideoSegmentThatComesChronologicallyAfterThisEpochTimeInSeconds(qint64 targetEpochTimeInSeconds, qint64 epochTimeInSecondsOfLatestVideoSegment)
{
    if(targetEpochTimeInSeconds >= epochTimeInSecondsOfLatestVideoSegment) //just in case xD
    {
        return determineLatestVideoSegmentPathOrUsePlaceholder();
    }

    QDateTime targetDateTime = QDateTime::fromMSecsSinceEpoch(targetEpochTimeInSeconds * 1000).toUTC();
    int latestVideoSegmentYear = QDateTime::fromMSecsSinceEpoch(epochTimeInSecondsOfLatestVideoSegment * 1000).toUTC().date().year();
    int targetYear = qBound(2014, targetDateTime.date().year(), latestVideoSegmentYear); //can't time travel...
    int determinedToExistYear = targetYear;
    QString determinedToExistYearQString = QString::number(targetYear);
    bool targetYearFound = true;
    while(!QFile::exists(m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + determinedToExistYearQString))
    {
        targetYearFound = false; //so we know to use the first vid of the first day of the next year, ez
        ++determinedToExistYear;
        if(determinedToExistYear > latestVideoSegmentYear) //can't time travel... this should never happen but eh already coded it
        {
            return determineLatestVideoSegmentPathOrUsePlaceholder();
        }
        determinedToExistYearQString = QString::number(determinedToExistYear);
    }

    //found a year folder
    QString absolutePathOfYearFolderWithSlashAppended = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + determinedToExistYearQString + QDir::separator();

    if(!targetYearFound)
    {
        return getFilePathOfFirstVideoSegmentInFirstDayOfYearFolder(absolutePathOfYearFolderWithSlashAppended);
    }

    //still in target epoch's year

    bool targetDayFound = true;
    int determinedToExistDayOfYearOfTarget = targetDateTime.date().dayOfYear();
    while(!QFile::exists(absolutePathOfYearFolderWithSlashAppended + QString::number(determinedToExistDayOfYearOfTarget)))
    {
        targetDayFound = false;
        ++determinedToExistDayOfYearOfTarget;
        if(determinedToExistDayOfYearOfTarget > targetDateTime.date().daysInYear())
        {
            //year shift guh, don't even know if next year folder exists
            //hmmmm..... maybe this isn't as bad as I thought... *continues thinkan*
            QDateTime zerothSecondOfFirstDayOfYearAfterTargetYear(QDate(targetDateTime.date().year()+1, 1, 1), QTime(0, 0), Qt::UTC);
            return determineFirstVideoSegmentThatComesChronologicallyAfterThisEpochTimeInSeconds(zerothSecondOfFirstDayOfYearAfterTargetYear.toMSecsSinceEpoch()/1000, epochTimeInSecondsOfLatestVideoSegment); //fuck yea recursion
        }
    }

    //found a day folder
    QString absolutePathOfDayFolderWithSlashAppended = absolutePathOfYearFolderWithSlashAppended + QString::number(determinedToExistDayOfYearOfTarget) + QDir::separator();

    if(!targetDayFound)
    {
        return getFilePathOfFirstVideoSegmentInDayFolder(absolutePathOfDayFolderWithSlashAppended);
    }

    //still in target epoch's day... but the target might be pointing to after all of the videos in that day, in which ase I think I need more recursion?
    //iterate the video segments in that day folder until you find one with a timestamp > the target timestamp (might never find it, in which case: recursion methinks)
    QDir dayDir(absolutePathOfDayFolderWithSlashAppended);
    QStringList videoSegmentsInDayFolder = dayDir.entryList((QDir::NoDotAndDotDot | QDir::Files), QDir::Time | QDir::Reversed);
    Q_FOREACH(const QString &currentVideoSegmentInDayFolder, videoSegmentsInDayFolder)
    {
        qint64 timestampInSecondsOfCurrentVideoSegment = determineSecondsSinceEpochFromVideoSegmentFilepath(currentVideoSegmentInDayFolder.toStdString()); //TODOoptional: senseless toStdString -> fromStdString conversion here
        if(timestampInSecondsOfCurrentVideoSegment > targetEpochTimeInSeconds)
        {
            //this is the one
            QString absolutePathOfFirstVideoSegmentAfterTargetEpochTime = absolutePathOfDayFolderWithSlashAppended + currentVideoSegmentInDayFolder;
            std::string absolutePathOfFirstVideoSegmentAfterTargetEpochTime_StdString = absolutePathOfFirstVideoSegmentAfterTargetEpochTime.toStdString();
            return absolutePathOfFirstVideoSegmentAfterTargetEpochTime_StdString;
        }
    }

    //no video segments in that folder were later than the target epoch time, so: day shift, possibly even year shift..... WHAT TO DO ;-) (JK I KNOW WHAT TO DO)
    //nope i need to use addDays to account for year shift: QDateTime zerothSecondOfNextDay(QDate(blahYear, blahDay+1))
    QDateTime zerothSecondOfNextDay = targetDateTime;
    zerothSecondOfNextDay.setTime(QTime(0, 0));
    zerothSecondOfNextDay.addDays(1);
    return determineFirstVideoSegmentThatComesChronologicallyAfterThisEpochTimeInSeconds(zerothSecondOfNextDay.toMSecsSinceEpoch()/1000, epochTimeInSecondsOfLatestVideoSegment); //fuck yea recursion

#if 0 //old, fail
    QDir yearDir(absolutePathOfYearFolderWithSlashAppended);
    QStringList daysInYearDir = yearDir.entryList((QDir::NoDotAndDotDot | QDir::Files), QDir::Time | QDir::Reversed);

    int indexOfDayFolderMaybe = daysInYearDir.indexOf(QString::number(targetDateTime.date().day()));
    if(indexOfDayFolderMaybe == -1)
    {

    }

    QString absolutePathOfDayFolderWithSlashAppended = absolutePathOfYearFolderWithSlashAppended + daysInYearDir.first() + QDir::separator();
    QDir dayDir(absolutePathOfDayFolderWithSlashAppended);
    QStringList videoSegmentsInDayFolder = dayDir.entryList((QDir::NoDotAndDotDot | QDir::Files), QDir::Time | QDir::Reversed);
    const QString &absoluteFilePathOfFirstVideoSegmentInDayFolder = absolutePathOfDayFolderWithSlashAppended + videoSegmentsInDayFolder.first();
    std::string firstVideoSegmentInDayFolderAsStdString = absoluteFilePathOfFirstVideoSegmentInDayFolder.toStdString();
    return firstVideoSegmentInDayFolderAsStdString;
#endif
}
string HackyVideoBullshitSiteGUI::getFilePathOfFirstVideoSegmentInFirstDayOfYearFolder(const QString &knownToExistYearFolderAbsolutePathWithSlashAppended)
{
    QDir aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt_Dir(knownToExistYearFolderAbsolutePathWithSlashAppended);
    const QStringList allDayFoldersInThatYearFolder = aYearFolderAfterYearThatJustEndedThatHasAtLeastOneVideoPublishedInIt_Dir.entryList((QDir::NoDotAndDotDot | QDir::Dirs), (QDir::Time));
    const QString &firstDayFolderInThatYearFolder = allDayFoldersInThatYearFolder.last(); //last is 'first' (choronologically) becuase we sort in reverse (I probably could/should change this, but I'm used to it (and kinda afraid of switching NOW) so whatever)
    QString firstDayFolderInThatYearFolder_AbsoluteAndWithSlashAppended = knownToExistYearFolderAbsolutePathWithSlashAppended + firstDayFolderInThatYearFolder + QDir::separator();
    QDir firstDayFolderInThatYearFolder_Dir(firstDayFolderInThatYearFolder_AbsoluteAndWithSlashAppended);
    const QStringList allVideoSegmentsInThatDayFolder = firstDayFolderInThatYearFolder_Dir.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Time));
    const QString &nextExistingVideoSegmentFilename = allVideoSegmentsInThatDayFolder.last();
    QString nextExistingVideoSegmentAbsoluteFilePath = firstDayFolderInThatYearFolder_AbsoluteAndWithSlashAppended + nextExistingVideoSegmentFilename;
    std::string nextExistingVideoSegmentAbsoluteFilePath_StdString = nextExistingVideoSegmentAbsoluteFilePath.toStdString();
    return nextExistingVideoSegmentAbsoluteFilePath_StdString;
}
string HackyVideoBullshitSiteGUI::getFilePathOfFirstVideoSegmentInDayFolder(const QString &knownToExistDayFolderAbsolutePathWithSlashAppended)
{
    QDir dayDir(knownToExistDayFolderAbsolutePathWithSlashAppended);
    const QStringList allVideoSegementsInThatDayFolder = dayDir.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Time));
    const QString &nextExistingVideoSegmentFilename = allVideoSegementsInThatDayFolder.last(); //last is first, because sorting in reverse xD
    QString nextExistingVideoSegmentFilename_Absolute = knownToExistDayFolderAbsolutePathWithSlashAppended + nextExistingVideoSegmentFilename;
    std::string nextExistingVideoSegmentFilename_Absolute_StdStr = nextExistingVideoSegmentFilename_Absolute.toStdString();
    return nextExistingVideoSegmentFilename_Absolute_StdStr;
}
void HackyVideoBullshitSiteGUI::handleHomeAnchorClickedSoShowLatestVideoSegmentEvenIfAlreadyHome()
{
   setInternalPath("/", true); //I'd imagine this is the same thing anchors do, but they optimizingly don't do it when the internal path hasn't changed. this, I think, forces it to go through my handleInternalPathChanged logic, which shows the latest video segment)
}
void HackyVideoBullshitSiteGUI::createCookieCrumbsFromPath(/*const string &internalPathInclFilename*/)
{
    //we already know it's a file and it exists

    m_CookieCrumbContainerWidget->clear();

    std::string internalPathEventuallyRebuilt = "/";
    std::string nextInternalPathPart = "";
    while((nextInternalPathPart = internalPathNextPart(internalPathEventuallyRebuilt)) != "")
    {
        WText *divider = new WText("/", Wt::XHTMLUnsafeText, m_CookieCrumbContainerWidget);
        divider->setMargin(WLength(3, WLength::Pixel), Wt::Left | Wt::Right);
        new WAnchor(WLink(WLink::InternalPath, internalPathEventuallyRebuilt + nextInternalPathPart), nextInternalPathPart, m_CookieCrumbContainerWidget);
        internalPathEventuallyRebuilt = internalPathEventuallyRebuilt + nextInternalPathPart + "/";
    }
#if 0
    QString asQstring = QString::fromStdString(internalPathInclFilename);
    QStringList pathParts = asQstring.split("/", QString::SkipEmptyParts, Qt::CaseSensitive);
    if(pathParts.size() < 2)
        return; //filename only
    std::string internalPathEventuallyRebuilt = "/";
    QStringList::const_iterator it = pathParts.constBegin();
    while(it != pathParts.constEnd())
    {
        QString currentPathPart = *it;
        WText *divider = new WText("/", Wt::XHTMLUnsafeText, m_CookieCrumbContainerWidget);
        divider->setMargin(WLength(3, WLength::Pixel), Wt::Left | Wt::Right);
        const std::string &currentPathPartStdString = currentPathPart.toStdString();
        new WAnchor(WLink(WLink::InternalPath, internalPathEventuallyRebuilt + currentPathPartStdString), currentPathPartStdString, m_CookieCrumbContainerWidget);
        internalPathEventuallyRebuilt = internalPathEventuallyRebuilt + currentPathPartStdString + "/";
        ++it;
    }
#endif
}
void HackyVideoBullshitSiteGUI::newTimelineIfNeededAndBringToFront()
{
    if(!m_TimeLineMyBrainWidget)
        m_TimeLineMyBrainWidget = new TimeLineWtWidget(m_TimelineAndDirectoryBrowsingStack);

    if(m_TimelineAndDirectoryBrowsingStack->currentWidget() != m_TimeLineMyBrainWidget)
    {
        m_TimelineAndDirectoryBrowsingStack->setCurrentWidget(m_TimeLineMyBrainWidget);
    }
}
void HackyVideoBullshitSiteGUI::deleteTimelineAndDirectoryBrowsingStackIfNeeded()
{
    if(m_TimelineAndDirectoryLogicalContainer)
    {
        delete m_TimelineAndDirectoryLogicalContainer; //implicitly deletes stack
        m_TimelineAndDirectoryLogicalContainer = 0;
    }

    //deleted implicitly because stack is parent, but still need to set to 0 for obvious reasons
    m_TimeLineMyBrainWidget = 0;
    m_BrowseMyBrainDirWidget = 0;
}
void HackyVideoBullshitSiteGUI::hvbs404()
{
    setMainContent(new WText("404 Not Found", Wt::XHTMLUnsafeText));
}
void HackyVideoBullshitSiteGUI::setMainContent(WWidget *contentToSet)
{
    //m_CentralBorderLayout->addWidget(contentToSet, WBorderLayout::Center);
    //m_CentralScrollArea->setWidget(contentToSet);
    //m_ContentsPlaceholder->addWidget(contentToSet);
    m_Contents = contentToSet; //bah, wtext wordwrapping text doesn't work when it's the child of a wcontainer -_-. pita..
    blahRootRedirect()->addWidget(contentToSet);
}
