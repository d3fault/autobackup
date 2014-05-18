#include "timelinewtwidget.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WDateTime>
#include <Wt/Utils>
#include <Wt/WFileResource>
#include <Wt/WAudio>
#include <Wt/WVideo>
#include <Wt/WImage>
#include <Wt/WVBoxLayout>
#include <Wt/WDateTime>

#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

#include "hvbsshared.h"

#define HVBS_ARBITRARY_BINARY_MIME_TYPE "application/octet-stream" //supposedly for unknown types i'm supposed to let the browser guess, but yea uhh what if it's an html file with javascripts inside of it? fuck you very much, application/octet + attachment forcing ftw. (dear internet people: you suck at standards. www is a piece of shit. i can do better (i will))

//TODOreq: renaming an item will make old links no longer valid, the system needs to not break when that happens. 404'ing is okay, but it would be even better if a nice message was presented (telling the user it wasn't their fault, there must have been a rename). this isn't that common of a case anyways (but i also don't want to tell myself not to rename just to keep urls valid -_- (but i also want to keep urls valid! fucking hell. i need a rename-aware system but that's gotta be revision-aware and yea not up for that just yet))

//TODOreq: i need to make sure that an outdated timestamp file (loaded into memory and shared via atomics) doesn't segfault if the user browses to a moved/renamed/deleted/etc file. this will happen when updating the folder using the move command hackery, since the latest/last timestamp values will still be in memory and will stay there until a 5 second timer sees that it exists (the move hackery completes) and reads the updated values from it. actually since the file is checked for actual existence before getting to the timeline widget, i think it might not even be an issue

//although i started off feeling like this was a disgusting hack (i almost used it for video segment presenting, but didn't because there's no easy way to get the 'next' video after that (atomic pointer could have changed twice!)), now that i've written most of it i actually kind of like it. the chances of segfault are ridiculously low, and i can easily make them lower if the fileset ever grows so large. this is a hacky video bullshit site, but someday i'd imagine it will be replaced by something with a proper ddb etc (i just need to either find or code one that meets my standards! ffff). maybe even a public dht ;-)
//5 minutes is so long it's like stupid long, and the memory that stays around that long is "not that big that it's significant at all but would be significant if i didn't free it every time shit changed (as in, if they were kept around)"

//I feel guilty for enjoying coding this. It's not like I've been, you know, putting off coding it for... well over 3 years... (MusicTimeline became ...)
QAtomicPointer<LastModifiedTimestampsAndPaths> *TimeLineWtWidget::m_LastModifiedTimestampsAndPaths = 0; //segfault if not set. performance performance performance! vroom.
void TimeLineWtWidget::setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer)
{
    m_LastModifiedTimestampsAndPaths = lastModifiedTimestampsSharedAtomicPointer;
}
TimeLineWtWidget::TimeLineWtWidget(WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory(0)
{
    //TODOoptional: next, previous (directory inode order)

    setContentAlignment(Wt::AlignLeft | Wt::AlignTop);

    WText *timelineControlsText = new WText("Timeline Controls - ", this);
    WColor myYellow(255, 255, 0);
    timelineControlsText->decorationStyle().setForegroundColor(myYellow);
    WAnchor *randomAnchor = new WAnchor(WLink(WLink::InternalPath, HVBS_WEB_CLEAN_URL_HACK_TO_MYBRAIN_TIMELINE), "Random Point In Time", this);
    randomAnchor->decorationStyle().setForegroundColor(WColor(0,255,0)); //bitches need visual cues because bitches is dumb as fuck n shit
    new WBreak(this);

    WContainerWidget *timelineControlsContainer = new WContainerWidget(this);
    timelineControlsContainer->setPadding(0);
    timelineControlsContainer->setMargin(0);
    WVBoxLayout *timelineControlsVBoxLayout = new WVBoxLayout(timelineControlsContainer);
    timelineControlsVBoxLayout->setContentsMargins(0, 0, 0, 0);
    timelineControlsVBoxLayout->setSpacing(0);

    WContainerWidget *earliestContainer = new WContainerWidget();
    earliestContainer->setContentAlignment(Wt::AlignLeft);
    WText *earliestText = new WText("Earliest: ", Wt::XHTMLUnsafeText, earliestContainer);
    earliestText->decorationStyle().setForegroundColor(myYellow);
    m_EarliestAnchor = new WAnchor(earliestContainer);
    m_EarliestAnchor->setTextFormat(Wt::PlainText);
    new WText(" ", Wt::XHTMLUnsafeText, earliestContainer);
    m_EarliestTimestamp = new WText(earliestContainer);
    m_EarliestTimestamp->setTextFormat(Wt::PlainText);
    m_EarliestAnchor->disable();
    timelineControlsVBoxLayout->addWidget(earliestContainer);

    WContainerWidget *previousContainer = new WContainerWidget();
    previousContainer->setContentAlignment(Wt::AlignLeft);
    WText *previousText = new WText("Previous: ", Wt::XHTMLUnsafeText, previousContainer);
    previousText->decorationStyle().setForegroundColor(myYellow);
    m_PreviousAnchor = new WAnchor(previousContainer);
    m_PreviousAnchor->setTextFormat(Wt::PlainText);
    new WText(" ", Wt::XHTMLUnsafeText, previousContainer);
    m_PreviousTimestamp = new WText(previousContainer);
    m_PreviousTimestamp->setTextFormat(Wt::PlainText);
    timelineControlsVBoxLayout->addWidget(previousContainer);

    WContainerWidget *currentContainer = new WContainerWidget();
    currentContainer->setContentAlignment(Wt::AlignCenter);
    WText *currentText = new WText("Current: ", Wt::XHTMLUnsafeText, currentContainer);
    currentText->decorationStyle().setForegroundColor(myYellow);
    m_CurrentAnchor = new WAnchor(currentContainer);
    m_CurrentAnchor->setTextFormat(Wt::PlainText);
    new WText(" ", Wt::XHTMLUnsafeText, currentContainer);
    m_CurrentTimestamp = new WText(currentContainer);
    m_CurrentTimestamp->setTextFormat(Wt::PlainText);
    timelineControlsVBoxLayout->addWidget(currentContainer);

    WContainerWidget *nextContainer = new WContainerWidget();
    nextContainer->setContentAlignment(Wt::AlignRight);
    m_NextTimestamp = new WText(nextContainer);
    m_NextTimestamp->setTextFormat(Wt::PlainText);
    new WText(" ", Wt::XHTMLUnsafeText, nextContainer);
    m_NextAnchor = new WAnchor(nextContainer);
    m_NextAnchor->setTextFormat(Wt::PlainText);
    WText *nextText = new WText(" :Next", Wt::XHTMLUnsafeText, nextContainer);
    nextText->decorationStyle().setForegroundColor(myYellow);
    timelineControlsVBoxLayout->addWidget(nextContainer);

    WContainerWidget *latestContainer = new WContainerWidget();
    latestContainer->setContentAlignment(Wt::AlignRight);
    m_LatestTimestamp = new WText(latestContainer);
    m_LatestTimestamp->setTextFormat(Wt::PlainText);
    new WText(" ", Wt::XHTMLUnsafeText, latestContainer);
    m_LatestAnchor = new WAnchor(latestContainer);
    m_LatestAnchor->setTextFormat(Wt::PlainText);
    WText *latestText = new WText(" :Latest", Wt::XHTMLUnsafeText, latestContainer);
    latestText->decorationStyle().setForegroundColor(myYellow);
    timelineControlsVBoxLayout->addWidget(latestContainer);
    m_LatestAnchor->disable();

    new WBreak(this);

    m_ContentsContainer = new WContainerWidget(this);
}
//if redirectToRandomPointInTimeline takes more than 5 minutes to execute (rofl), we will segfault. TODOoptimization: the value 5 minutes can be shrunk to some value that is related to the average execution times (worst case scenario lookups (so... last in list? idfk)) when the server is running at maximum connection load (10k). 5 minutes is probably a ridiculously high estimate. the amount of connections needed to slow the execution down to 5 minutes is ridiculous, especially considering it would reflect the user experience of waiting 5 minutes for a page load (in which case scale horizontal is warranted)
void TimeLineWtWidget::redirectToRandomPointInTimeline()
{
    LastModifiedTimestampsAndPaths *sortedLastModifiedTimestamps = m_LastModifiedTimestampsAndPaths->loadAcquire(); //pointer only guaranteed to be valid for minimum 5 minutes after the loadAquire (rofl so ugly hack but idgaf), so never keep it around, always load again <3
    static unsigned int randomSeed = 0; //not thread safe and VERY likely to get weird undefined values, but actually it doesn't matter at all since it's just being used for a qsrand call (all undefined results would still fall within the range of the unsigned int, and hell i could even argue that the thread unsafety actually ADDS to the randomness muahahaha), fuck the police :). this is without a doubt my favorite hack i've ever done... so ugly and yet still "safe" in an awesome way. come to think of it, i don't even need the datetime or raw unique id shit anymore...
    WApplication *wApplication = WApplication::instance();
    qsrand(++randomSeed);
    //qsrand(wApplication->rawUniqueId() + static_cast<unsigned int>(WDateTime::currentDateTime().toTime_t()) + (++randomSeed)); //TODOoptimization: since q[s]rand are threadsafe, they probably use locking and so are a bottleneck. i wish i could store a boost rng in thread-specific-storage just like WApplication::instance() is. yea just decided it's worth asking on wt forums <3, would have tons of good uses aside from just this
    unsigned randomPointInTimeline = (qrand() % sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->size()); //TODOoptiona; random_int_distribution might be warranted here, since it's so big... but it's not critical or anything so fuck it for now

    wApplication->setInternalPath(sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(randomPointInTimeline)->Path, true); //TODOoptional: maybe redirect instead of setInternalPath? reason = seo. but tbh idfk...
    //TODOoptional: use video segments in selection of random file
}
//if presentFile takes more than 5 minutes to execute (rofl), we will segfault
void TimeLineWtWidget::presentFile(const QString &relativePath_aka_internalPathQString, const QString &absolutePath /*already checked for existence on fs, but the file may have been deleted/renamed since*/, const std::string &myBrainItemFilenameOnlyStdString)
{
    m_ContentsContainer->clear();

    if(relativePath_aka_internalPathQString.startsWith(HVBS_WEB_CLEAN_URL_TO_AIRBORNE_VIDEO_SEGMENTS "/")) //video segment hack (because it's not in .lastModifiedTimestamps
    {
        //earliest/latest left as is to be leik ahn entree pointz toin le tiemenlienen plz i guezz

        m_PreviousAnchor->setText("TODO");
        m_PreviousAnchor->setLink(WLink());
        m_PreviousAnchor->disable();
        m_PreviousTimestamp->setText("");

        m_NextAnchor->setText("TODO");
        m_NextAnchor->setLink(WLink());
        m_NextAnchor->disable();
        m_NextTimestamp->setText("");

        m_CurrentAnchor->setLink(WLink(WLink::InternalPath, relativePath_aka_internalPathQString.toStdString()));
        m_CurrentAnchor->setText(myBrainItemFilenameOnlyStdString);
        QFileInfo blahFileInfo(relativePath_aka_internalPathQString); //filename is timestamp, but make it human readable
        const QString &videoSegmentFilenameOnly = blahFileInfo.fileName();
        const QString &videoSegmentFilenameWithoutExt = videoSegmentFilenameOnly.left(videoSegmentFilenameOnly.lastIndexOf("."));
        bool convertOk = false;
        long long videoSegmentTimestamp = videoSegmentFilenameWithoutExt.toLongLong(&convertOk);
        if(convertOk)
        {
            m_CurrentTimestamp->setText(longLongTimestampToWString(videoSegmentTimestamp));
        }
        else
        {
            m_CurrentTimestamp->setText("error");
        }
    }
    else //not video segment hack (REGULAR)
    {
        LastModifiedTimestampsAndPaths *sortedLastModifiedTimestamps = m_LastModifiedTimestampsAndPaths->loadAcquire();
        if(!sortedLastModifiedTimestamps)
        {
            new WText("500 internal server error", m_ContentsContainer);
            return;
        }
        if(m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory != sortedLastModifiedTimestamps)
        {
            m_LatestAnchor->disable(); //when shit changes, the latest anchor is stale (unless timestamp file wasn't updated properly). disabling it makes it get an up to date value
            m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory = sortedLastModifiedTimestamps;
        }
        //thinking out loud (typing out loud (typing my thoughts)): lol mom interrupted me and then i had lunch since writing that prefix, uhh what was i on about? oh right i need to be able to look up based on path and timestamp, so wtf data structure am i going to use?!?!? i also need to be able to access it by index so i can use the 'random' functionality. that's 3 primary keys by my count, wtfz!?!? simply doing it is easy, but doing it fast/right will require some thinkan. a QList<teimstompIeteim> would give me random and in order tiemstompz, but then looking up by path is expensive. a qhash<path> gives me fast lookups by path, but [unless i do a linked list of em (next,previous)] doesn't give me in order shiz. i either can't have my cookie and eat it too, or perhaps i should just use two lists!?!? i don't need to do anything with the path (since it is... a path) except find out previous and next. so wtf i am contradicting myself here: find by path = use an hash, but hash has no ordering so fuck. also doesn't appear that hash can be accessed index-wise so now random'ing is fucked. a hash<path,int> containing just an index into a sorted list as it's value would work, so long as both lists are gotten via the same loadAquire call (and are therefore synchronized (two back to back loadAquires wouldn't be)... and idk it's just kinda like i'm maintaining a whole nother list just to use it as a primary key. wtf was that shit i read about having something with two primary keys? there's generic containers for that i believe... *turns on internet box*... now hmm do i want boost.multiindex or boost.bimap... they sound the same to me except apparently multiindex depends on bimap... so.... *reads moar*...
        //seems bimap is sufficient, not entirely sure but multiindex looks overkill. still i wonder if keeping second hash<path,indexIntoList> would be FASTER (not necessarily more memory conservative), simply because bimap only does maps :(. i want a left: map, right: hash :(. maybe such thing would be dumb and/or maybe such thing is already possible, *keeps reading*
        //fuck yea boost/bimap/unordered_set_of might be just that! *reads*
        //shit, bimap needs unique keys, whereas i have tons of timestamp collisions :(
        //fuck it, to KISS ima just use QList<item> (sorted by timestamp and then again by path) and QHash<path, listIndex> and just keep them synchronized myself, with a comment saying an optimization using bimap or SIMILAR is probably possible...

        //we've already QDir::clean'd it and established that it starts with our internal path, but the timeline internal path has not been chopped off yet...

        //old (file already verified to exist on fs):
        //this is probably a faster 404 check than a fs read, but i could be wrong. i think it'll be faster only because it's not a kernel call. i'm ALREADY doing an atomic load, so that [kernel call] is free. i'd imagine that the fs cache (kernel) is a hash as well, so the speeds would be identical aside from the kernel call (which i've heard are expensive but honestly i have no clue :-P (expensive is a relative word, so relative to WHAT!?!? blah now i see why premature optimization is the biggest waste of time ever. so many nerds on the internet describing operations as "expensive", but they give little/no comparison/relativity)). i could be wrong and the fs cache check could still be faster, perhaps because it uses an algorithm more suited to directories/files (it walks the directories? idfk. but i mean a fucking hash should be "splitting" (sharding) after the very first character, so shit maybe my hash is way faster :)

        try
        {
            const std::string &relativePath_aka_internalPath = relativePath_aka_internalPathQString.toStdString();
            uint indexIntoFlatSortedList = sortedLastModifiedTimestamps->PathsIndexIntoFlatListHash->at(relativePath_aka_internalPath);

            if(indexIntoFlatSortedList > 0)
            {
                //there is a 'previous', so yea make previous and maybe make earliest
                TimestampAndPath *previousTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(indexIntoFlatSortedList-1);
                const std::string &previousPath = previousTimestampAndPath->Path;
                QFileInfo previousPathFileInfo(QString::fromStdString(previousPath)); //TODOreq (since they are relative and current dir isn't where they live, it's very unlikely (but they might still TRY fff (had it marked as done, now removed again))): find out if these touch the hard drive. if they do, i'll chop off the fucking filename myself!
                m_PreviousAnchor->setDisabled(false);
                m_PreviousAnchor->setLink(WLink(WLink::InternalPath, previousPath));
                m_PreviousAnchor->setText(previousPathFileInfo.fileName().toStdString());
                m_PreviousTimestamp->setText(longLongTimestampToWString(previousTimestampAndPath->Timestamp));

                if(m_EarliestAnchor->isDisabled())
                {
                    //TODOoptional: solve the problem of earliest being stale if i ever modify first (making it no longer earliest). for now fuck it because that's so incredibly rare (however, the same problem applies to "latest" (for latest i can just disable the anchor whenever the pointer changes (ONLY keep pointer around to detect changing, NEVER use it otherwise)...)
                    TimestampAndPath *earliestTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->first();
                    const std::string &earliestPath = earliestTimestampAndPath->Path;
                    QFileInfo earliestPathFileInfo(QString::fromStdString(earliestPath));
                    m_EarliestAnchor->setDisabled(false);
                    m_EarliestAnchor->setLink(WLink(WLink::InternalPath, earliestPath));
                    m_EarliestAnchor->setText(earliestPathFileInfo.fileName().toStdString());
                    m_EarliestTimestamp->setText(longLongTimestampToWString(earliestTimestampAndPath->Timestamp));
                }
            }
            else
            {
                //no previous
                m_PreviousAnchor->setText("...");
                m_PreviousAnchor->setLink(WLink());
                m_PreviousAnchor->setDisabled(true);
                m_PreviousTimestamp->setText("");

                m_EarliestAnchor->setText("No Earlier");
                m_EarliestAnchor->setLink(WLink());
                m_EarliestAnchor->setDisabled(true);
                m_EarliestTimestamp->setText("");
            }

            m_CurrentAnchor->setLink(WLink(WLink::InternalPath, relativePath_aka_internalPath));
            m_CurrentAnchor->setText(myBrainItemFilenameOnlyStdString);
            m_CurrentTimestamp->setText(longLongTimestampToWString(sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(indexIntoFlatSortedList)->Timestamp));

            if(indexIntoFlatSortedList < static_cast<uint>(sortedLastModifiedTimestamps->PathsIndexIntoFlatListHash->size()-1))
            {
                //there is a 'next', so [...]
                TimestampAndPath *nextTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(indexIntoFlatSortedList+1);
                const std::string &nextPath = nextTimestampAndPath->Path;
                QFileInfo nextPathFileInfo(QString::fromStdString(nextPath));
                m_NextAnchor->setDisabled(false);
                m_NextAnchor->setLink(WLink(WLink::InternalPath, nextPath));
                m_NextAnchor->setText(nextPathFileInfo.fileName().toStdString());
                m_NextTimestamp->setText(longLongTimestampToWString(nextTimestampAndPath->Timestamp));

                if(m_LatestAnchor->isDisabled())
                {
                    TimestampAndPath *latestTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->last();
                    const std::string &latestPath = latestTimestampAndPath->Path;
                    QFileInfo latestPathFileInfo(QString::fromStdString(latestPath));
                    m_LatestAnchor->setDisabled(false);
                    m_LatestAnchor->setLink(WLink(WLink::InternalPath, latestPath));
                    m_LatestAnchor->setText(latestPathFileInfo.fileName().toStdString());
                    m_LatestTimestamp->setText(longLongTimestampToWString(latestTimestampAndPath->Timestamp));
                }
            }
            else
            {
                //no next
                m_NextAnchor->setText("...");
                m_NextAnchor->setLink(WLink());
                m_NextAnchor->setDisabled(true);
                m_NextTimestamp->setText("");

                m_LatestAnchor->setText("No Later");
                m_LatestAnchor->setLink(WLink());
                m_LatestAnchor->setDisabled(true);
                m_LatestTimestamp->setText("");
            }
        }
        catch(std::out_of_range &pathNotInHashException)
        {
            //the file exists on the fs, but not in the lastModifiedTimestamps file. this happens when a file is renamed/deleted (semi-OT: what's more important? SEO (consistency) or overall organization (neatness)?). it's a race condition where the file existed so we got past that check but since it's renamed and a new checkout/etc since then, it no longer exists either in that path or in the last modified timestamps
            new WText("The file you requested may have been renamed or deleted. Try looking for it in the directory (which may also have been renamed/deleted) it used to be in: ", m_ContentsContainer);
            QFileInfo currentItemFileInfo(relativePath_aka_internalPathQString);
            new WAnchor(WLink(WLink::InternalPath, currentItemFileInfo.path().toStdString() + "/"), "Here", m_ContentsContainer); //TODOoptional: keep cd-up'ing until the directory exists
            return;
        }
    }


    //now that the previous/next/etc buttons have been enabled/disabled, time to present the item
    WPushButton *downloadButton = new WPushButton(HVBS_DOWNLOAD_LOVE, m_ContentsContainer);
    new WBreak(m_ContentsContainer);

    const std::string &mimeType = embedBasedOnFileExtensionAndReturnMimeType(absolutePath);

    WFileResource *downloadResource = new WFileResource(mimeType, absolutePath.toStdString(), downloadButton);
    downloadResource->suggestFileName(myBrainItemFilenameOnlyStdString, WResource::Attachment);
    downloadButton->setResource(downloadResource);
}
void TimeLineWtWidget::embedPicture(const string &mimeType, const QString &filename)
{
    const std::string &filenameStdString = filename.toStdString();
    WAnchor *pictureAnchor = new WAnchor();
    WFileResource *pictureResource = new WFileResource(mimeType, filenameStdString, pictureAnchor);
    pictureAnchor->setLink(WLink(pictureResource));
    pictureAnchor->setImage(new WImage(WLink(pictureResource), filenameStdString));
    pictureAnchor->setTarget(TargetNewWindow);
    setMainContent(pictureAnchor);
}
void TimeLineWtWidget::embedVideoFile(const string &mimeType, const QString &filename)
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
void TimeLineWtWidget::embedAudioFile(const string &mimeType, const QString &filename)
{
    WContainerWidget *audioPlayerContainer = new WContainerWidget();
    WAudio *audioPlayer = new WAudio(audioPlayerContainer);
    setMainContent(audioPlayerContainer);
    audioPlayer->setOptions(WAbstractMedia::Autoplay | WAbstractMedia::Controls);
    WFileResource *audioFileResource = new WFileResource(mimeType, filename.toStdString(), audioPlayer);
    audioPlayer->addSource(WLink(audioFileResource), mimeType);
    audioPlayer->setAlternativeContent(new WText(HVBS_NO_HTML5_AUDIO_OR_ERROR, Wt::XHTMLUnsafeText));
}
string TimeLineWtWidget::embedBasedOnFileExtensionAndReturnMimeType(const QString &filename)
{
    const QString &filenameToLower = filename.toLower();

    //PICTURES
    if(filenameToLower.endsWith(".webp"))
    {
        std::string webpMime = "image/webp";
        embedPicture(webpMime, filename);
        return webpMime;
    }
    if(filenameToLower.endsWith(".jpg") || filenameToLower.endsWith(".jpeg"))
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
    if(filenameToLower.endsWith(".svg"))
    {
        std::string svgMime = "image/svg+xml";
        embedPicture(svgMime, filename);
        return svgMime;
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
                setMainContent(new WText("500 Internal Server Error"));
                return std::string(HVBS_ARBITRARY_BINARY_MIME_TYPE);
            }
            QTextStream textFileStream(&textFile);
            textFileString = textFileStream.readAll();
        }
        //WTextArea *textArea = new WTextArea(WString::fromUTF8((const char *)textFileString.toUtf8()));
        //blahSetContent(textArea);
        //textArea->setReadOnly(true);

        WString htmlEncoded = Wt::Utils::htmlEncode(WString::fromUTF8((const char *)textFileString.toUtf8())); //TODOoptimization: do this on the copy stored to disk (but obviously not the master copy)
        //Wt::Utils::removeScript(htmlEncoded);
        WText *textDoc = new WText("<pre>" + htmlEncoded + "</pre>", Wt::XHTMLUnsafeText);
        setMainContent(textDoc);
        return std::string("text/plain");
    }
    //TODOreq: zzzz

    setMainContent(new WText("No web-view available for this type of file, but you can still download it", Wt::XHTMLUnsafeText));
    return std::string(HVBS_ARBITRARY_BINARY_MIME_TYPE);
}
void TimeLineWtWidget::setMainContent(WWidget *mainContent)
{
    m_ContentsContainer->addWidget(mainContent);
}
WString TimeLineWtWidget::longLongTimestampToWString(long long timestamp)
{
    return WDateTime::fromTime_t(static_cast<std::time_t>(timestamp)).toString(); //TODOreq: i think my timestamps are in .lastModifiedTimestamps are AZT, but I think WDateTime thinks they are GMT. But actually maybe not since QDateTime::toMsecsSinceEpoch would be GMT (i need to digg into QuickDirtyAutoBackupHalper and find out what it uses). IDFK but tbh I want to move permanently and with every software I write to GMT. I even want to change my clocks to GMT. No offsets, no DST (shoot whoever invented it), etc. Yea yea it's silly to say "shoot whoever invented time ZONES" (they were invented before it was known they are invented), but it's still stupid we aren't all using GMT worldwide by now. Idiots fear change and it would be a religious war before MURRICA changed to Eurofag tiemzohnez...
}
