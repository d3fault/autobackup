#include "timelinewtwidget.h"

#include <Wt/WApplication>
#include <Wt/WGroupBox>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WText>

#include <QFileInfo>

//TODOreq: renaming an item will make old links no longer valid, the system needs to not break when that happens. 404'ing is okay, but it would be even better if a nice message was presented (telling the user it wasn't their fault, there must have been a rename). this isn't that common of a case anyways (but i also don't want to tell myself not to rename just to keep urls valid -_- (but i also want to keep urls valid! fucking hell. i need a rename-aware system but that's gotta be revision-aware and yea not up for that just yet))

//although i started off feeling like this was a disgusting hack (i almost used it for video segment presenting, but didn't because there's no easy way to get the 'next' video after that (atomic pointer could have changed twice!)), now that i've written most of it i actually kind of like it. the chances of segfault are ridiculously low, and i can easily make them lower if the fileset ever grows so large. this is a hacky video bullshit site, but someday i'd imagine it will be replaced by something with a proper ddb etc (i just need to either find or code one that meets my standards! ffff). maybe even a public dht ;-)
//5 minutes is so long it's like stupid long, and the memory that stays around that long is "not that big that it's significant at all but would be significant if i didn't free it every time shit changed (as in, if they were kept around)"

//I feel guilty for enjoying coding this. It's not like I've been, you know, putting off coding it for... well over 3 years... (MusicTimeline became ...)
QAtomicPointer<LastModifiedTimestampsAndPaths> *TimeLineWtWidget::m_LastModifiedTimestampsAndPaths = 0; //segfault if not set. performance performance performance! vroom.
QString TimeLineWtWidget::m_MyOwnInternalPath = "";
void TimeLineWtWidget::setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer)
{
    m_LastModifiedTimestampsAndPaths = lastModifiedTimestampsSharedAtomicPointer;
}
void TimeLineWtWidget::setTimelineInternalPath(const QString &timelineInternalPath)
{
    m_MyOwnInternalPath = timelineInternalPath;
}
TimeLineWtWidget::TimeLineWtWidget(WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory(0)
{
    //TODOreq: bread crumb paths lead to dir view
    //TODOreq: dir view and timeline(item) view both in stack, so hitting back goes back to proper page of dir view
    //TODOreq: first, last, next, previous (chronological)
    //TODOoptional: next, previous (directory alphabetical)

    setContentAlignment(Wt::AlignLeft | Wt::AlignTop);

    m_EarliestAnchor = new WAnchor(this);
    m_EarliestAnchor->setTextFormat(Wt::PlainText);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
    m_PreviousAnchor = new WAnchor(this);
    m_PreviousAnchor->setTextFormat(Wt::PlainText);
    m_PreviousAnchor->setDisabled(true);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
    m_PermalinkToThisAnchor = new WAnchor(this);
    m_PermalinkToThisAnchor->setTextFormat(Wt::PlainText);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
    m_RandomAnchor = new WAnchor(this);
    m_RandomAnchor->setTextFormat(Wt::PlainText);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
    m_NextAnchor = new WAnchor(this);
    m_NextAnchor->setTextFormat(Wt::PlainText);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
    m_LatestAnchor = new WAnchor(this);
    m_LatestAnchor->setTextFormat(Wt::PlainText);
    m_LatestAnchor->setDisabled(true);
    new WText(" - ", Wt::XHTMLUnsafeText, this);
}
//if handleInternalPathChanged takes more than 5 minutes to execute (rofl), we will segfault
void TimeLineWtWidget::handleInternalPathChanged(const QString &newInternalPath)
{
    if(newInternalPath == m_MyOwnInternalPath)
    {
        //not specifying == random point in time <3
        LastModifiedTimestampsAndPaths *sortedLastModifiedTimestamps = m_LastModifiedTimestampsAndPaths->loadAcquire(); //pointer only guaranteed to be valid for minimum 5 minutes after the loadAquire (rofl so ugly hack but idgaf), so never keep it around, always load again <3
        WApplication *wApplication = WApplication::instance();
        unsigned randomPointInTimeline = (wApplication->rawUniqueId() % sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->size()); //TODOoptiona; random_int_distribution might be warranted here, since it's so big... but it's not critical or anything so fuck it for now

        wApplication->setInternalPath(sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(randomPointInTimeline)->Path, true); //TODOreq: maybe redirect instead of setInternalPath? reason = seo. but tbh idfk...
        return;
    }

    //getting the item is easy since we already have it's path (TODOreq: make sure it exists before proceeding), but we still need to find the "previous" and "next" (TODOreq: both can not exist) in our timestamp datathing
    LastModifiedTimestampsAndPaths *sortedLastModifiedTimestamps = m_LastModifiedTimestampsAndPaths->loadAcquire();
    if(!sortedLastModifiedTimestamps) //TODOreq: make sure it's ready before starting wt, blockingqueued bullshit
    {
        //500 internal server error
        return;
    }
    if(m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory != sortedLastModifiedTimestamps)
    {
        m_LatestAnchor->disable(); //when shit changes, the latest anchor is stale (unless timestamp file wasn't updated properly)
        m_PointerToDetectWhenTheShitChangesBut_DO_NOT_USE_THIS_because_it_might_point_to_freed_memory = sortedLastModifiedTimestamps;
    }
    //thinking out loud (typing out loud (typing my thoughts)): lol mom interrupted me and then i had lunch since writing that prefix, uhh what was i on about? oh right i need to be able to look up based on path and timestamp, so wtf data structure am i going to use?!?!? i also need to be able to access it by index so i can use the 'random' functionality. that's 3 primary keys by my count, wtfz!?!? simply doing it is easy, but doing it fast/right will require some thinkan. a QList<teimstompIeteim> would give me random and in order tiemstompz, but then looking up by path is expensive. a qhash<path> gives me fast lookups by path, but [unless i do a linked list of em (next,previous)] doesn't give me in order shiz. i either can't have my cookie and eat it too, or perhaps i should just use two lists!?!? i don't need to do anything with the path (since it is... a path) except find out previous and next. so wtf i am contradicting myself here: find by path = use an hash, but hash has no ordering so fuck. also doesn't appear that hash can be accessed index-wise so now random'ing is fucked. a hash<path,int> containing just an index into a sorted list as it's value would work, so long as both lists are gotten via the same loadAquire call (and are therefore synchronized (two back to back loadAquires wouldn't be)... and idk it's just kinda like i'm maintaining a whole nother list just to use it as a primary key. wtf was that shit i read about having something with two primary keys? there's generic containers for that i believe... *turns on internet box*... now hmm do i want boost.multiindex or boost.bimap... they sound the same to me except apparently multiindex depends on bimap... so.... *reads moar*...
    //seems bimap is sufficient, not entirely sure but multiindex looks overkill. still i wonder if keeping second hash<path,indexIntoList> would be FASTER (not necessarily more memory conservative), simply because bimap only does maps :(. i want a left: map, right: hash :(. maybe such thing would be dumb and/or maybe such thing is already possible, *keeps reading*
    //fuck yea boost/bimap/unordered_set_of might be just that! *reads*
    //shit, bimap needs unique keys, whereas i have tons of timestamp collisions :(
    //fuck it, to KISS ima just use QList<item> (sorted by timestamp and then again by path) and QHash<path, listIndex> and just keep them synchronized myself, with a comment saying an optimization using bimap or SIMILAR is probably possible...

    //we've already QDir::clean'd it and established that it starts with our internal path, but the timeline internal path has not been chopped off yet...

    //this is probably a faster 404 check than a fs read, but i could be wrong. i think it'll be faster only because it's not a kernel call. i'm ALREADY doing an atomic load, so that [kernel call] is free. i'd imagine that the fs cache (kernel) is a hash as well, so the speeds would be identical aside from the kernel call (which i've heard are expensive but honestly i have no clue :-P (expensive is a relative word, so relative to WHAT!?!? blah now i see why premature optimization is the biggest waste of time ever. so many nerds on the internet describing operations as "expensive", but they give little/no comparison/relativity)). i could be wrong and the fs cache check could still be faster, perhaps because it uses an algorithm more suited to directories/files (it walks the directories? idfk. but i mean a fucking hash should be "splitting" (sharding) after the very first character, so shit maybe my hash is way faster :)
    uint indexIntoFlatSortedList;
    try
    {
        indexIntoFlatSortedList = sortedLastModifiedTimestamps->PathsIndexIntoFlatListHash->at(newInternalPath.toStdString());

        if(indexIntoFlatSortedList > 0)
        {
            //there is a 'previous', so yea make previous and maybe make earliest
            TimestampAndPath *previousTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(indexIntoFlatSortedList-1);
            const std::string &previousPath = previousTimestampAndPath->Path;
            QFileInfo previousPathFileInfo(QString::fromStdString(previousPath)); //TODOreq: find out if these touch the hard drive. if they do, i'll chop off the fucking filename myself!
            m_PreviousAnchor->setDisabled(false);
            m_PreviousAnchor->setLink(WLink(WLink::InternalPath, previousPath));
            m_PreviousAnchor->setText(previousPathFileInfo.fileName().toStdString());

            if(m_EarliestAnchor->isDisabled())
            {
                //TODOoptional: solve the problem of earliest being stale if i ever modify first (making it no longer earliest). for now fuck it because that's so incredibly rare (however, the same problem applies to "latest" (for latest i can just disable the anchor whenever the pointer changes (ONLY keep pointer around to detect changing, NEVER use it otherwise)...)
                TimestampAndPath *earliestTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->first();
                const std::string &earliestPath = earliestTimestampAndPath->Path;
                QFileInfo earliestPathFileInfo(QString::fromStdString(earliestPath));
                m_EarliestAnchor->setDisabled(false);
                m_EarliestAnchor->setLink(WLink(WLink::InternalPath, earliestPath));
                m_EarliestAnchor->setText(earliestPathFileInfo.fileName().toStdString());
            }
        }
        else
        {
            //no previous
            m_PreviousAnchor->setText("...");
            m_PreviousAnchor->setLink(WLink());
            m_PreviousAnchor->setDisabled(true);

            m_EarliestAnchor->setText("No Earlier");
            m_EarliestAnchor->setLink(WLink());
            m_EarliestAnchor->setDisabled(true);
        }

        //TODOreq: present current

        if(indexIntoFlatSortedList < static_cast<uint>(sortedLastModifiedTimestamps->PathsIndexIntoFlatListHash->size()-1))
        {
            //there is a 'next', so [...]
            TimestampAndPath *nextTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->at(indexIntoFlatSortedList+1);
            const std::string &nextPath = nextTimestampAndPath->Path;
            QFileInfo nextPathFileInfo(QString::fromStdString(nextPath));
            m_NextAnchor->setDisabled(false);
            m_NextAnchor->setLink(WLink(WLink::InternalPath, nextPath));
            m_NextAnchor->setText(nextPathFileInfo.fileName().toStdString());

            if(m_LatestAnchor->isDisabled())
            {
                TimestampAndPath *latestTimestampAndPath = sortedLastModifiedTimestamps->SortedTimestampAndPathFlatList->last();
                const std::string &latestPath = latestTimestampAndPath->Path;
                QFileInfo latestPathFileInfo(QString::fromStdString(latestPath));
                m_LatestAnchor->setDisabled(false);
                m_LatestAnchor->setLink(WLink(WLink::InternalPath, latestPath));
                m_LatestAnchor->setText(latestPathFileInfo.fileName().toStdString());
            }
        }
        else
        {
            //no next
            m_NextAnchor->setText("...");
            m_NextAnchor->setLink(WLink());
            m_NextAnchor->setDisabled(true);

            m_LatestAnchor->setText("No Later");
            m_LatestAnchor->setLink(WLink());
            m_LatestAnchor->setDisabled(true);
        }
    }
    catch(std::out_of_range &pathNotInHashException)
    {
        //TODOreq: 404, the file may have been renamed...
        return;
    }
}
