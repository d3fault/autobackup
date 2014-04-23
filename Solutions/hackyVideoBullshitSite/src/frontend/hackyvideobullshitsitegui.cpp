#include "hackyvideobullshitsitegui.h"

#include <Wt/WServer>
#include <Wt/WAudio>
#include <Wt/WVideo>
#include <Wt/WImage>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WLink>
#include <Wt/WEnvironment>
#include <Wt/WFileResource>
#include <Wt/WPushButton>
#include <Wt/WPanel>
#include <Wt/Utils>
#include <Wt/WCssStyleSheet>

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDate>
#include <QFileInfo>

#define HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER "/some/placeholder/video/TODOreq.ogg" //TODOoptional: when no year/day folders are present (error imo) i could add code to present this... and it could even server as a pre-launch kind... of... countdown... thingo... (nah (ok changed my mind, yah (since it was a simple patch 'if year == 2013' xD)))
#define HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Videos/Airborne"
#define HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN "/Browse"
#define HVBS_WEB_CLEAN_URL_HACK_TO_DOWNLOAD_MYBRAIN "/Download"
#define HVBS_BROWSE_ANDOR_DOWNLOAD_TOOLTIP "Proceed with caution. What is seen can never be unseen"
#define HVBS_BROWSE_MY_BRAIN_STRING "Browse My Brain Online"
#define HVBS_DOWNLOAD_MY_BRAIN_IN_FULL_STRING "Download My Brain In Full"
#define HVBS_ARBITRARY_BINARY_MIME_TYPE "application/octet-stream" //supposedly for unknown types i'm supposed to let the browser guess, but yea uhh what if it's an html file with javascripts inside of it? fuck you very much, application/octet + attachment forcing ftw. (dear internet people: you suck at standards. www is a piece of shit. i can do better (i will))
#define HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL "https://anonymousbitcoincomputing.com/advertising/buy-ad-space/d3fault/0"
#define HVBS_NO_HTML_MEDIA_OR_ERROR(mediaType) "Either your browser is a piece of shit and doesn't support HTML5 " #mediaType " (You should use Mozilla Firefox), or there was an error establishing a connection to the " #mediaType " stream"
#define HVBS_NO_HTML5_VIDEO_OR_ERROR HVBS_NO_HTML_MEDIA_OR_ERROR(video)
#define HVBS_NO_HTML5_AUDIO_OR_ERROR HVBS_NO_HTML_MEDIA_OR_ERROR(audio)
#define HVBS_DOWNLOAD_LOVE \
    "Download " \
    "/ Save " \
    "/ Fukken Save " \
    "/ Keep " \
    "/ Steal " \
    "/ Borrow " \
    "/ Use " \
    "/ Absorb " \
    "/ Own " \
    "/ Assimilate" \
    "/ Incorporate " \
    "/ Memorize " \
    "/ Multiply " \
    "/ Archive " \
    "/ Persist " \
    "/ Cache " \
    "/ Hoard " \
    "/ Receive " \
    "/ Replicate " \
    "/ Reproduce" \
    "/ Grab " \
    "/ Get " \
    "/ Copy"
//having way too much fun with this

//segfault if server is started before assigning these that are pointers :-P (fuck yea performance)
AdImageGetAndSubscribeManager* HackyVideoBullshitSiteGUI::m_AdImageGetAndSubscribeManager = 0;
QString HackyVideoBullshitSiteGUI::m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended = QDir::rootPath(); //root tends to have a small number of files that rarely change, so a good default for when the user forgets to specify (since it will be 'watched')
QString HackyVideoBullshitSiteGUI::m_MyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended = QDir::homePath(); //internal path is concatenated onto this, and already has a slash at the beginning
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
void HackyVideoBullshitSiteGUI::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended)
{
    m_MyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended = myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended;
}
void HackyVideoBullshitSiteGUI::setCopyrightText(const string &copyrightText)
{
    m_CopyrightText = "<pre>" + Wt::Utils::htmlEncode(copyrightText) + "</pre>"; //TODOoptimization: could pre-htmlEncode and even color'ify (code) the various docs beforehand, instead of on-demand like i'm doing now (well, i'm not doing the coloring at all atm)
}
void HackyVideoBullshitSiteGUI::setDplLicenseText(const string &dplLicenseText)
{
    m_DplLicenseText = "<pre>" + Wt::Utils::htmlEncode(dplLicenseText) + "</pre>";
}
HackyVideoBullshitSiteGUI::HackyVideoBullshitSiteGUI(const WEnvironment &env)
    : WApplication(env)
    , m_AdImageContainer(0)
    , m_ContentsHeaderRow(0)
    , m_Contents(0)
    , m_NoJavascriptAndFirstAdImageChangeWhichMeansRenderingIsDeferred(!env.ajax())
{
    QMetaObject::invokeMethod(m_AdImageGetAndSubscribeManager, "getAndSubscribe", Qt::QueuedConnection, Q_ARG(AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*, static_cast<AdImageGetAndSubscribeManager::AdImageSubscriberIdentifier*>(this)), Q_ARG(std::string, sessionId()), Q_ARG(GetAndSubscriptionUpdateCallbackType, boost::bind(&HackyVideoBullshitSiteGUI::handleAdImageChanged, this, _1, _2, _3)));

    setTitle("I've been living the past few years as if there were tiny cameras all around me. The only path to sanity is to make that definitely true");

    setCssTheme("polished");

    styleSheet().addRule("body", "background-color: black; color: white; font-family: arial; font-size: large");
    styleSheet().addRule("a:link", "color: #e1e1e1;");
    styleSheet().addRule("a:visited", "color: #ffa2a2;");
    styleSheet().addRule("pre", "white-space: pre-wrap; white-space: -moz-pre-wrap; white-space: -pre-wrap; white-space: -o-pre-wrap; word-wrap: break-word;"); //fucking hate css, not as much as js but (had:way) more than html (except when css saves the day from htmls shortcomings...). on that note, fuck the rule saying you can't use an apostrophe s unless it means "<something> is" (unless used with proper noun(?)). "htmls" looks fucking retarded. html owns the shortcomings, therefore posessive, who cares if it's not a proper noun (or is it? fuck if i know guh all subjective bullshit anyways. i'm going for clarity, suck my dick)

    //TODOreq: text files downloaded copies have copyright.txt at top. I'm thinking my 'master' branch has copyright.txt prepend thing always at top, and another separate branch is what I work on (script prepends and merges/pushes/whatevers into master). master because it should be the default for anyone that checks it out... but i don't want to permanently put that shit at the top of mine because it will annoy the fuck out of me (especially since i don't want the EMBED copy to have it since it is way sexier to have it in a WPanel). It could be called the allrightsreserved branch xD. Note: not that it matters, but I think I'd need to be constantly creating a temporary branch, running the prepender, committing, and then... err... i think rebasing ONTO master? idfk lol... i suck at git
    //but also collaboration and merging etc will mean i have to deal with licenses anyways. for code i don't care that much tbh, but for text files that aren't even that long.... fuuuuuuck i don't want stupid headers prepended on all of em. BUT honestly they're easy to both insert and remove via scripting so... (lol at the bug where i 'remove' the text and then it removes it from the file that i used as input to tell me what to remove (easily fixed by pulling it back out of git history (or a skip file exception) but still i'm predicting it will happen :-P)

    //TODOreq: timestamps
    //TODOreq: MyBrain increments(?)
    //TODOoptional: folder (recursive) saving... but how would i do that, zip on demand? more importantly, how would i limit it?
    //TODOoptional: "random"
    //TODOoptional: ad image placeholder takes up dimensions, so no "popping" and content shifting when it finally loads (shit annoys the FUCK out of me, but eh almost every desktop environment is guilty of it as well (highly considering changing to one of those tile based ones... (more likely to code one myself xD (but eh implementing freedesktop protocols sounds cumbersome))))
    //TODOoptional: when "no web view", show list of desktop apps <--> extensions mapping

    m_AdImagePositionPlaceholder = new WContainerWidget(root());
    m_AdImagePositionPlaceholder->setContentAlignment(Wt::AlignCenter | Wt::AlignTop);

    WPanel *copyrightDropDown = new WPanel(root());
    copyrightDropDown->setCollapsible(true);
    copyrightDropDown->setCollapsed(true);
    copyrightDropDown->setTitle("Copyright (C) 2014 Steven Curtis Wieler II <http://d3fault.net/> -- All Freedoms Preserved -- Click here for more information about your right to copy");
    WText *copyrightText = new WText(m_CopyrightText, Wt::XHTMLUnsafeText);
    copyrightText->decorationStyle().setBackgroundColor(WColor(0,0,0));
    copyrightText->decorationStyle().setForegroundColor(WColor(255,255,255));
    copyrightDropDown->setCentralWidget(copyrightText);
    //copyrightDropDown->decorationStyle().setCursor(PointingHandCursor);

    WPanel *dplDropDown = new WPanel(root());
    dplDropDown->setCollapsible(true);
    dplDropDown->setCollapsed(true);
    dplDropDown->setTitle("d3fault public license, version 3+ -- Click here to read the legal text");
    WText *dplText = new WText(m_DplLicenseText, Wt::XHTMLUnsafeText);
    dplText->decorationStyle().setBackgroundColor(WColor(0,0,0));
    dplText->decorationStyle().setForegroundColor(WColor(255,255,255));
    dplDropDown->setCentralWidget(dplText);
    //dplDropDown->decorationStyle().setCursor(PointingHandCursor);

    new WAnchor(WLink(WLink::Url, "https://bitcoin.org/en/faq"), "Bitcoin", root());
    new WText(" donation address: 1FwZENuqEHHNCAz4fiWbJWSknV4BhWLuYm", Wt::XHTMLUnsafeText, root());
    new WBreak(root());

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
    //layout defaults (empty m_ContentsHeaderRow, m_Contents pointing to zero)
    if(m_ContentsHeaderRow)
        delete m_ContentsHeaderRow;
    m_ContentsHeaderRow = new WContainerWidget(root());

    if(m_Contents)
    {
        delete m_Contents;
        m_Contents = 0; //this isn't used [anymore] to "add the content into/onto", but is now just a pointer to the content added DIRECTLY to root(). reasoning: WText word wrapping doesn't work when it's a child of WContainerWidget, but i still need a way to delete it (hence, pointer)
    }
    //m_Contents = new WContainerWidget(root());


    if(newInternalPath == "/" || newInternalPath == "" || newInternalPath == HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/Latest")
    {
        std::string latestVideoSegmentFilePath = determineLatestVideoSegmentPathOrUsePlaceholder();
        //latestVideoSegmentFilePath is either set to most recent or to placeholder

        WAnchor *browseMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN), HVBS_BROWSE_MY_BRAIN_STRING, m_ContentsHeaderRow);
        browseMyBrainAnchor->setToolTip(HVBS_BROWSE_ANDOR_DOWNLOAD_TOOLTIP);
        browseMyBrainAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));
        browseMyBrainAnchor->setTarget(TargetNewWindow);
        //olo: browseEverythingAnchor->decorationStyle().setTextDecoration(WCssDecorationStyle::Blink);

        new WText(" or ", Wt::XHTMLUnsafeText, m_ContentsHeaderRow);

        WAnchor *downloadMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_DOWNLOAD_MYBRAIN), HVBS_DOWNLOAD_MY_BRAIN_IN_FULL_STRING, m_ContentsHeaderRow);
        downloadMyBrainAnchor->setToolTip(HVBS_BROWSE_ANDOR_DOWNLOAD_TOOLTIP);
        downloadMyBrainAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));
        downloadMyBrainAnchor->setTarget(TargetNewWindow);

        new WBreak(m_ContentsHeaderRow);

        WPushButton *downloadButton = new WPushButton(HVBS_DOWNLOAD_LOVE, m_ContentsHeaderRow);
        //TODOreq: "Link to this: ", needs to chop off base dir from absolute path to make clean url...
        new WBreak(m_ContentsHeaderRow);

        //TODOreq: previous button
        WPushButton *nextVideoClipPushButton = new WPushButton("Next Clip", m_ContentsHeaderRow); //if next != current; aka if new-current != current-when-started-playing
        new WBreak(m_ContentsHeaderRow);

        nextVideoClipPushButton->clicked().connect(this, &HackyVideoBullshitSiteGUI::handleNextVideoClipButtonClicked);

        WContainerWidget *videoContainer = new WContainerWidget(); //derp alternate content is not deleted when the WVideo is
        WVideo *videoPlayer = new WVideo(videoContainer);
        setMainContent(videoContainer);
        WFileResource *latestVideoSegmentFileResource = new WFileResource("video/ogg", latestVideoSegmentFilePath, videoPlayer);
        QFileInfo fileInfo(QString::fromStdString(latestVideoSegmentFilePath));
        const std::string &filenameOnly = fileInfo.fileName().toStdString();
        latestVideoSegmentFileResource->suggestFileName(filenameOnly, WResource::Attachment);
        videoPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
        videoPlayer->addSource(WLink(latestVideoSegmentFileResource), "video/ogg");
        downloadButton->setResource(latestVideoSegmentFileResource);
        //videoPlayer->resize(720, 480);
        videoPlayer->resize(800, 600);
        videoPlayer->setAlternativeContent(new WText(HVBS_NO_HTML5_VIDEO_OR_ERROR, Wt::XHTMLUnsafeText));
        if(!environment().ajax())
        {
            return;
        }
        videoPlayer->ended().connect(this, &HackyVideoBullshitSiteGUI::handleLatestVideoSegmentEnded);
        return;
    }
    std::string rewrittenInternalPath = newInternalPath;
    if(newInternalPath == HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN) //Note: a file called "Browse" will never be viewed xD
    {
        rewrittenInternalPath = "";
    }
    else if(newInternalPath == HVBS_WEB_CLEAN_URL_HACK_TO_DOWNLOAD_MYBRAIN) //Note: a file called "Download" will never be viewed :-P
    {
        //TODOreq: host and link to the torrent files (tpb is good enough placeholder for now, fuck it)
        WContainerWidget *downloadContainer = new WContainerWidget();

        WAnchor *downloadMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN), HVBS_BROWSE_MY_BRAIN_STRING, downloadContainer);
        downloadMyBrainAnchor->setToolTip(HVBS_BROWSE_ANDOR_DOWNLOAD_TOOLTIP);
        downloadMyBrainAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));

        new WBreak(downloadContainer);

        new WBreak(downloadContainer);
        new WText("Download in full:", Wt::XHTMLUnsafeText, downloadContainer);
        new WBreak(downloadContainer);
        WAnchor *tpbMyBrainPublicFilesAnchor = new WAnchor(WLink(WLink::Url, "http://thepiratebay.se/torrent/9754200/My_Brain_-_Public_Files"), "My Brain - Public Files", downloadContainer);
        tpbMyBrainPublicFilesAnchor->setTarget(TargetNewWindow);

        new WBreak(downloadContainer);

        new WText("Hold onto this for me plz:", Wt::XHTMLUnsafeText, downloadContainer);
        new WBreak(downloadContainer);
        WAnchor *tpbMyBrainPrivateFilesAnchor = new WAnchor(WLink(WLink::Url, "http://thepiratebay.se/torrent/9754217/My_Brain_-_Private_Files"), "My Brain - Private Files", downloadContainer);
        tpbMyBrainPrivateFilesAnchor->setTarget(TargetNewWindow);

        setMainContent(downloadContainer);

        return;
    }

    QString theInternalPathCleanedQString = QDir::cleanPath(QString::fromStdString(rewrittenInternalPath)); //strips trailing slash if dir
    if(theInternalPathCleanedQString.contains("/..", Qt::CaseSensitive)) //even if i didn't have the right half of this or statment, the items they would see would be un-navigable because clicking them would go to a "../" (left half) internal path
    {
        hvbs404();
        //quit();
        return;
    }

    //TODOreq: post launch files should have a drop watch folder thingo too i suppose...

    const QString &myBrainItemToPresentAbsolutePathQString = m_MyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended + theInternalPathCleanedQString;
    if(QFile::exists(myBrainItemToPresentAbsolutePathQString))
    {
        WAnchor *downloadMyBrainAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_DOWNLOAD_MYBRAIN), HVBS_DOWNLOAD_MY_BRAIN_IN_FULL_STRING, m_ContentsHeaderRow);
        downloadMyBrainAnchor->setToolTip(HVBS_BROWSE_ANDOR_DOWNLOAD_TOOLTIP);
        downloadMyBrainAnchor->decorationStyle().setForegroundColor(WColor(0, 255, 0));

        new WBreak(m_ContentsHeaderRow);

        QFileInfo myBrainItemFileInfo(myBrainItemToPresentAbsolutePathQString);
        const std::string myBrainItemToPresentAbsolutePathStdString = myBrainItemToPresentAbsolutePathQString.toStdString();
        const std::string &theInternalPathCleanedStdString = theInternalPathCleanedQString.toStdString();
        if(myBrainItemFileInfo.isFile())
        {
            WPushButton *downloadButton = new WPushButton(HVBS_DOWNLOAD_LOVE, m_ContentsHeaderRow);
            new WBreak(m_ContentsHeaderRow);

            new WText("Link to this: ", Wt::XHTMLUnsafeText, m_ContentsHeaderRow);
            const std::string &filenameOnly = myBrainItemFileInfo.fileName().toStdString();
            new WAnchor(WLink(WLink::InternalPath, theInternalPathCleanedStdString), filenameOnly, m_ContentsHeaderRow);
            new WBreak(m_ContentsHeaderRow);

            const std::string &mimeType = embedBasedOnFileExtensionAndReturnMimeType(myBrainItemToPresentAbsolutePathQString);

            WFileResource *downloadResource = new WFileResource(mimeType, myBrainItemToPresentAbsolutePathStdString, downloadButton);
            downloadResource->suggestFileName(filenameOnly, WResource::Attachment);
            //TODoreq: maybe set filename, but maybe WFileResource does this itself. done ish but not for segments viewing
            downloadButton->setResource(downloadResource);
            return;
        }
        else if(myBrainItemFileInfo.isDir())
        {
            //TODOoptimization: pagination of dir shitz (maybe a #page thingo to keep urls clean while still allowing page specification?). maybe QDirIterator + pagination might be what I want, even though I lose soring :-/...
            //TODOoptimization: minddump is like 1800 entries (and growing) so uhm......... maybe a special case for it? idfk. ls->sort->ls->sort xD

            std::string upOneLevel;

            {
                const QString upOneLevelCleaned = QDir::cleanPath(theInternalPathCleanedQString + "/../"); //if i ever find a solution for mybrain root dir listing, this should obviously not be shown
                upOneLevel = upOneLevelCleaned.toStdString();
                if(upOneLevel == "/")
                {
                    upOneLevel = HVBS_WEB_CLEAN_URL_HACK_TO_BROWSE_MYBRAIN;
                }
                else if(upOneLevel == "/..")
                {
                    upOneLevel = "/";
                }
            }

            WContainerWidget *directoryBrowsingContainerWidget = new WContainerWidget();
            new WBreak(directoryBrowsingContainerWidget);
            WAnchor *upOneFolderAnchor = new WAnchor(WLink(WLink::InternalPath, upOneLevel), "Go up one folder level", directoryBrowsingContainerWidget);
            upOneFolderAnchor->decorationStyle().setForegroundColor(WColor(77, 92, 207));
            new WBreak(directoryBrowsingContainerWidget);

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
            setMainContent(directoryBrowsingContainerWidget);
            return;
        }
        //fuck symlinks etc
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

    if(m_AdImageContainer)
        delete m_AdImageContainer; //delete/replace the previous one, if there was one (TODOoptimization: maybe just setImage/setUrl/etc instead?)
    m_AdImageContainer = new WContainerWidget(m_AdImagePositionPlaceholder);
    m_AdImageContainer->setContentAlignment(Wt::AlignCenter | Wt::AlignTop);

    if(newAdUrl == "n") //hack. both newAdImageResource and newAdAltAndHover are undefined if url is "n" (they may be 0/empty, or they may still be 'yesterdays')
    {
        //set up "no ad" placeholder
        WImage *placeholderAdImage = new WImage(WLink(WLink::Url, /*http://d3fault.net*/"/no.ad.placeholder.jpg"), "Buy this ad space for BTC 0.00001");
        placeholderAdImage->resize(576, 96);
        m_AdImageAnchor = new WAnchor(WLink(WLink::Url, HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL), placeholderAdImage, m_AdImageContainer);
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
    m_AdImageAnchor = new WAnchor(WLink(WLink::Url, newAdUrl), adImage, m_AdImageContainer);
    m_AdImageAnchor->setTarget(TargetNewWindow);
    new WBreak(m_AdImageContainer);
    new WAnchor(WLink(WLink::Url, HVBS_ABC2_BUY_D3FAULT_CAMPAIGN_0_URL), "Buy this ad space", m_AdImageContainer);

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
        bool yearFolderFound = QFile::exists(m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + QString::number(currentDate.year())); //woot sorting problem solved for root folder ls SORT'ing
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
            const QString &dayFolderToLookFor = m_AirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL_withSlashAppended + QString::number(currentDate.year()) + QDir::separator() + QString::number(currentDate.dayOfYear()) + QDir::separator();
            bool dayFolderFound = QFile::exists(dayFolderToLookFor); //TODOreq: as we are subtracting years, would it maybe make dayOfYear change as well in the leap year case (whoever invented leap years should be shot)
            if(!dayFolderFound)
            {
                currentDate = currentDate.addDays(-1);
                continue;
            }
            //day folder found, now find latest segment using sorting :(
            QDir dayFolder(dayFolderToLookFor);
            const QStringList all3MinuteSegmentsInDayFolder = dayFolder.entryList((QDir::NoDotAndDotDot | QDir::Files), (QDir::Name | QDir::Reversed));
            return all3MinuteSegmentsInDayFolder.isEmpty() ? HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER /*perhaps should also prevent the year/day folder spinlock described above */ : (dayFolderToLookFor + all3MinuteSegmentsInDayFolder.first()).toStdString(); //TODOoptimization: read the if'd out stuff above about fixing having to list (cached.) + sort (probably not cached?) the dir. basically ~11:59pm will be more expensive than ~12:01am
        }
    }
    //should never get here, but just in case and to make compiler stfu:
    return HVBS_PRELAUNCH_OR_NO_VIDEOS_PLACEHOLDER;
}
void HackyVideoBullshitSiteGUI::embedPicture(const string &mimeType, const QString &filename)
{
    const std::string &filenameStdString = filename.toStdString();
    WAnchor *pictureAnchor = new WAnchor();
    WFileResource *pictureResource = new WFileResource(mimeType, filenameStdString, pictureAnchor);
    pictureAnchor->setLink(WLink(pictureResource));
    pictureAnchor->setImage(new WImage(WLink(pictureResource), filenameStdString));
    pictureAnchor->setTarget(TargetNewWindow);
    setMainContent(pictureAnchor);
}
void HackyVideoBullshitSiteGUI::embedVideoFile(const string &mimeType, const QString &filename)
{
    WContainerWidget *videoContainer = new WContainerWidget();
    WVideo *videoPlayer = new WVideo(videoContainer);
    setMainContent(videoContainer);
    WFileResource *videoFileResource = new WFileResource(mimeType, filename.toStdString(), videoPlayer);
    videoFileResource->setDispositionType(WResource::Inline);
    videoPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
    videoPlayer->addSource(WLink(videoFileResource), mimeType);
    videoPlayer->setAlternativeContent(new WText(HVBS_NO_HTML5_VIDEO_OR_ERROR, Wt::XHTMLUnsafeText));
}
void HackyVideoBullshitSiteGUI::embedAudioFile(const string &mimeType, const QString &filename)
{
    WContainerWidget *audioPlayerContainer = new WContainerWidget();
    WAudio *audioPlayer = new WAudio(audioPlayerContainer);
    setMainContent(audioPlayerContainer);
    audioPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
    WFileResource *audioFileResource = new WFileResource(mimeType, filename.toStdString(), audioPlayer);
    audioPlayer->addSource(WLink(audioFileResource), mimeType);
    audioPlayer->setAlternativeContent(new WText(HVBS_NO_HTML5_AUDIO_OR_ERROR, Wt::XHTMLUnsafeText));
}
string HackyVideoBullshitSiteGUI::embedBasedOnFileExtensionAndReturnMimeType(const QString &filename)
{
    const QString &filenameToLower = filename.toLower();

    //PICTURES
    if(filenameToLower.endsWith(".webp"))
    {
        std::string webpMime = "image/webp";
        embedPicture(webpMime, filename);
        return webpMime;
    }
    if(filenameToLower.endsWith(".jpeg") || filenameToLower.endsWith(".jpg"))
    {
        std::string jpegMime = "image/jpeg";
        embedPicture(jpegMime, filename);
        return jpegMime;
    }
    if(filenameToLower.endsWith(".gif"))
    {
        std::string gifMime = "image/gif";
        embedPicture(gifMime, filename);
        return gifMime;
    }
    if(filenameToLower.endsWith(".png"))
    {
        std::string pngMime = "image/png";
        embedPicture(pngMime, filename);
        return pngMime;
    }

    //VIDEO
    if(filenameToLower.endsWith(".webm"))
    {
        std::string webmMime = "video/webm";
        embedVideoFile(webmMime, filename);
        return webmMime;
    }
    if(filenameToLower.endsWith(".ogg") || filenameToLower.endsWith(".ogv"))
    {
        std::string oggMime = "video/ogg";
        embedVideoFile(oggMime, filename);
        return oggMime;
    }
    if(filenameToLower.endsWith(".mkv"))
    {
        std::string mkvMime = "video/x-matroska";
        embedVideoFile(mkvMime, filename);
        return mkvMime;
    }
    //was tempted to do avi/wmv/etc, but the chances of them even playing in a browser approaches zero...

    //AUDIO
    if(filenameToLower.endsWith(".opus"))
    {
        std::string opusMime = "audio/opus";
        embedAudioFile(opusMime, filename);
        return opusMime;
    }
    if(filenameToLower.endsWith(".oga"))
    {
        std::string ogaMime = "audio/ogg";
        embedAudioFile(ogaMime, filename);
        return ogaMime;
    }
    //ditto with the audio xD

    //TEXT
    if(filenameToLower.endsWith(".txt") || filenameToLower.endsWith(".c") || filenameToLower.endsWith(".cpp") || filenameToLower.endsWith(".h") || filenameToLower.endsWith(".html") || filenameToLower.endsWith(".php") || filenameToLower.endsWith(".phps") || filenameToLower.endsWith(".s") || filenameToLower.endsWith(".java") || filenameToLower.endsWith(".xml") || filenameToLower.endsWith(".bat") || filenameToLower.endsWith(".sh") || filenameToLower.endsWith(".ini") || filenameToLower.endsWith(".pri") || filenameToLower.endsWith(".pro"))
    {
        QString textFileString;
        {
            QFile textFile(filename);
            if(!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                //TODOreq: 500 internal server errror
                return std::string(HVBS_ARBITRARY_BINARY_MIME_TYPE);
            }
            QTextStream textFileStream(&textFile);
            textFileString = textFileStream.readAll();
        }
        //WTextArea *textArea = new WTextArea(WString::fromUTF8((const char *)textFileString.toUtf8()));
        //blahSetContent(textArea);
        //textArea->setReadOnly(true);

        WString htmlEncoded = Wt::Utils::htmlEncode(WString::fromUTF8((const char *)textFileString.toUtf8()));
        //Wt::Utils::removeScript(htmlEncoded);
        WText *textDoc = new WText("<pre>" + htmlEncoded + "</pre>", Wt::XHTMLUnsafeText);
        setMainContent(textDoc);
        return std::string("text/plain");
    }
    //TODOreq: zzzz

    setMainContent(new WText("No web-view available for this type of file, but you can still download it", Wt::XHTMLUnsafeText));
    return std::string(HVBS_ARBITRARY_BINARY_MIME_TYPE);
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
    root()->addWidget(contentToSet);
}
