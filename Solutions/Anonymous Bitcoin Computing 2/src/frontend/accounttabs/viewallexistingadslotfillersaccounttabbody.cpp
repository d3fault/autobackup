#include "viewallexistingadslotfillersaccounttabbody.h"

#include <Wt/WFileResource>
#include <Wt/Utils>

#include <boost/filesystem.hpp>

#include <fstream>

#include "../anonymousbitcoincomputingwtgui.h"

#include "abc2couchbaseandjsonkeydefines.h"

#define VIEW_ALL_AD_SLOT_FILLERS_PATH_TO_HDD_PERMACACHE_WITH_TRAILING_SLASH "/tmp/" //TODOportable
#define VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE 10

//all pages are 1 index based, even though the db stuff backing it is not

//TODOoptimization: for js, we should triggerUpdate the images as they come. no-js just stays deferRendered until the last one...
//TODOoptimization: to defend against DDOS, we can delete all our in memory SingleUseSelfDeletingMemoryResource for the 'page changing from' when changing to a new page. It is exploitable (even accidentally, by.. say... lynx) because the large images stay in memory until they are requested. Deleting them, whether they were used or not, on page change, is one way to help fight this (but ultimately, many sessions/tabs open gets right around that defense so guh). There's also the threading issue of resources being served concurrently, not even sure I can delete them (look in SingleUseSelfDeletingMemoryResource's destructor for more info about that) xD...
ViewAllExistingAdSlotFillersAccountTabBody::ViewAllExistingAdSlotFillersAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp), m_CurrentPageOneIndexBased(1)
{ }
void ViewAllExistingAdSlotFillersAccountTabBody::populateAndInitialize()
{
    m_AbcApp->getCouchbaseDocumentByKeyBegin(adSpaceAllSlotFillersKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETALLADSLOTFILLERSDOCFORVIEWING;

    //TODOoptimization: re-organize other methods all throughout app so that the db hit is scheduled/in-progress before doing any other stuff (that the db hit doesn't depend on obviously), like for example the below object instantiations and call to deferRendering...
    m_AbcApp->deferRendering();
    new WText("<b>View all your previously uploaded advertisements</b>", this);
    new WBreak(this);
    //TODOoptional: refresh buttan to pull in new uploads during the same session (low priority, since they already have a preview after upload..), OR uploading one can 'push' it directly over to us (after checking we aren't NULL or something). That pushing over strategy wouldn't work if they have two tabs/sessions open, but oh well
}
void ViewAllExistingAdSlotFillersAccountTabBody::attemptToGetAllAdSlotFillersFinished(const string &allAdSlotFillersJsonDocMaybe, bool lcbOpSuccess, bool dbError)
{
    //it is a latency optimization to keep all slot fillers doc around, it is a memory optimization to not keep all slot fillers doc around
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        cerr << "attemptToGetAllAdSlotFillersFinished db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //all ad slot fillers doc not existing indicates no ad slot fillers uploaded yet...
        new WBreak(this);
        new WText("You haven't any uploaded any ads yet. Click the 'New Advertisement' tab above to do so", this);
        m_AbcApp->resumeRendering();
        return;
    }

    //got all ad slot fillers doc, so now show them the first page. we KNOW there is at least one ad slot filler on the first page...

    ptree pt;
    std::istringstream is(allAdSlotFillersJsonDocMaybe);
    read_json(is, pt);

    std::string adsCountString = pt.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT);
    m_AdsCount = boost::lexical_cast<int>(adsCountString);
    m_TotalPagesCount = static_cast<int>(ceil(static_cast<double>(m_AdsCount) / static_cast<double>(VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE)));

    //[First-button][Previous-button] Page [spinbox-showing-current-page] [Go-button] of %numPages% [Next-button][Last-button] ...... where First/Previous/Next/Last are enabled/disabled as appropriate

    new WText("Advertisements uploaded: " + adsCountString, this);
    new WBreak(this);

    m_FirstPageButton = new WPushButton("First Page", this);
    m_FirstPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::firstPageButtonClicked);
    m_FirstPageButton->disable(); //starts disabled because on first page

    new WText(" ", this); //TODOoptimization: litter Wt::XHTMLUnsafeText anywhere there's a string literal

    m_PreviousPageButton = new WPushButton("Previous Page", this);
    m_PreviousPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::previousPageButtonClicked);
    m_PreviousPageButton->disable(); //ditto

    new WText(" Page: ", this);

    m_PageSpinbox = new WSpinBox(this);
    m_PageSpinbox->setRange(1, m_TotalPagesCount);
    m_PageSpinbox->setValue(1);
    m_PageSpinbox->enterPressed().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::goToPageButtonClicked);

    new WText(" ", this);

    WPushButton *goToPageButton = new WPushButton("Go", this);
    goToPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::goToPageButtonClicked);

    new WText(" of " + boost::lexical_cast<std::string>(m_TotalPagesCount) + " pages. ", this);

    m_NextPageButton = new WPushButton("Next Page", this);
    m_NextPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::nextPageButtonClicked);
    m_NextPageButton->setEnabled(m_TotalPagesCount > 1);

    new WText(" ", this);

    m_LastPageButton = new WPushButton("Last Page", this);
    m_LastPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::lastPageButtonClicked);
    m_LastPageButton->setEnabled(m_TotalPagesCount > 1);


    new WBreak(this);
    m_AllAdSlotFillersPagesStack = new WStackedWidget(this); //this was in the constructor, but we want it to be below the page controls (not that it matters since rendering is deferred still)

    buildCurrentPageAndAddToStackAndPageHash(); //had this earlier as "async dispatch start. results will still go below controls", but now moving to below the controls because it depends on m_AllAdSlotFillersPagesStack being already created now :-P
}
void ViewAllExistingAdSlotFillersAccountTabBody::buildCurrentPageAndAddToStackAndPageHash()
{
    m_NumberOfAdsHackilyMultiGetting = VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE; //sensible defaults (was zero)
    if(m_CurrentPageOneIndexBased == m_TotalPagesCount)
    {
        //first and last pages may have uneven numbers (what's that fancy word i'm looking for...)
        m_NumberOfAdsHackilyMultiGetting = (m_AdsCount % VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE);
        if(m_NumberOfAdsHackilyMultiGetting == 0) //but maybe they don't ;-P
        {
            m_NumberOfAdsHackilyMultiGetting = VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE;
        }
    }

    //zero based indexes here (my head kind of hurts)
    int startDbIndex = ((m_CurrentPageOneIndexBased-1) * VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE);
    int endDbIndex = startDbIndex + m_NumberOfAdsHackilyMultiGetting;
    m_AdSlotFillersOnePageOrderingHash.clear();
    m_PointerToCurrentPageInConstruction = new WContainerWidget(m_AllAdSlotFillersPagesStack);
    for(int i = startDbIndex; i < endDbIndex; ++i)
    {
        const std::string &keyToCurrentAdSlotFiller = adSpaceSlotFillerKey(m_AbcApp->m_CurrentlyLoggedInUsername, boost::lexical_cast<std::string>(i));
#if 0
        const std::string &sha1OfkeyToCurrentAdSlotFiller = Wt::Utils::sha1(keyToCurrentAdSlotFiller);
        const std::string &permaCachePath = VIEW_ALL_AD_SLOT_FILLERS_PATH_TO_HDD_PERMACACHE_WITH_TRAILING_SLASH + sha1OfkeyToCurrentAdSlotFiller;
        if(boost::filesystem::exists(permaCachePath)) //TODOreq: race condition if two sessions are open and the same page (set of ads) are requested simultaneously. would be racy for whether or not the file exists (so they probably need to be MOVED INTO POSITION atomically after filled for starters). by the time our db hit comes back, the file might exist (or the file might be in progress of being created (and that in progress of being created may have even failed (but eh fuck it tbh)). basically: use temp file using "sha1(key) + WApp::sessionId", and after it's filled check for existence again before moving into position (if exist, just delete that temp file we just created). TODOoptimization: fails can be periodically detected by seeing if any filenames with the sessionId (indicating temp) exist and if their last modified date was a while ago (ex: yesterday)
            //TODOoptimization: perma-cache does not save us from having to get the b64 image (but it could with db refactor), perma-cache saves us from memory exhaustion from unrequested images staying in memory indefinitely (as long as session is open (if the user tells their browser not to request images))
        {
            //perma-cache (hdd) hit!
            WContainerWidget *currentAdImageContainer = new WContainerWidget(m_PointerToCurrentPageInConstruction);
            setupAdImagePreview(currentAdImageContainer, permaCachePath);
            //--m_NumberOfAdsHackilyMultiGetting; //TODOreq

        }
        else
        {
            //perma-cache (hdd) miss!
#endif
            m_AbcApp->getCouchbaseDocumentByKeyBegin(keyToCurrentAdSlotFiller);
            m_AdSlotFillersOnePageOrderingHash[keyToCurrentAdSlotFiller] = new WContainerWidget(m_PointerToCurrentPageInConstruction); //empty placeholder, setting up the ordering here/now. the hash is to be able to access it again later to populate it
        //}
    }
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::HACKYMULTIGETAPAGEWORTHOFADSLOTFILLERS; //came up with this while sleeping, should work...
}
void ViewAllExistingAdSlotFillersAccountTabBody::oneAdSlotFillerFromHackyMultiGetAttemptFinished(const std::string &keyToAdSlotFillerArriving, const string &oneAdSlotFillerJsonDoc, bool lcbOpSuccess, bool dbError)
{
    --m_NumberOfAdsHackilyMultiGetting; //TODOreq: we definitely still want to decrement this even if dbError or lcbOpFail, but err ehh uhmm idk i need some kind of special handling of that shit... like what if only one out of the 10 fails? or all 10 fail? etc
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "oneAdSlotFillerFromHackyMultiGetAttemptFinished db error" << endl;

        if(m_NumberOfAdsHackilyMultiGetting == 0)
        {
            m_AbcApp->resumeRendering();
        }
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        cerr << "TOTAL SYSTEM FAILURE: an ad slot that all ad slot fillers said existed, didn't, during: oneAdSlotFillerFromHackyMultiGetAttemptFinished" << endl;
        if(m_NumberOfAdsHackilyMultiGetting == 0)
        {
            m_AbcApp->resumeRendering();
        }
        return;
    }

    //got one ad slot filler, add to current page

    ptree pt;
    std::istringstream is(oneAdSlotFillerJsonDoc);
    read_json(is, pt);

    //every ad slot filler sets up an image
    WContainerWidget *adImageAnchorOrderingPlaceholderContainer = m_AdSlotFillersOnePageOrderingHash.at(keyToAdSlotFillerArriving);
    new WBreak(adImageAnchorOrderingPlaceholderContainer);
    new WBreak(adImageAnchorOrderingPlaceholderContainer);
    new WText("Preview of '" + base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_NICKNAME)) + "':", adImageAnchorOrderingPlaceholderContainer);
    new WBreak(adImageAnchorOrderingPlaceholderContainer);

    std::pair<string,string> guessedExtensionAndMimeType = StupidMimeFromExtensionUtil::guessExtensionAndMimeType(pt.get<std::string>(JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION));
    //SingleUseSelfDeletingMemoryResource *adImageResource = new SingleUseSelfDeletingMemoryResource(base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_IMAGEB64)), "image" + guessedExtensionAndMimeType.first, "image/" + guessedExtensionAndMimeType.second, WResource::Inline, adImageAnchorOrderingPlaceholderContainer);

    const std::string &permaCacheFilenameOnly = Wt::Utils::base64Encode(keyToAdSlotFillerArriving);
    const std::string permaCacheFilePath = VIEW_ALL_AD_SLOT_FILLERS_PATH_TO_HDD_PERMACACHE_WITH_TRAILING_SLASH + permaCacheFilenameOnly;

    //JIT perma-cachify
    if(!boost::filesystem::exists(permaCacheFilePath))
    {
        const std::string &tempFilenameSoNotClashingWithNeighborSessions_AndAlsoForExistenceAtomicity = VIEW_ALL_AD_SLOT_FILLERS_PATH_TO_HDD_PERMACACHE_WITH_TRAILING_SLASH  "TEMP_" + permaCacheFilenameOnly + "_" + Wt::Utils::base64Encode(m_AbcApp->sessionId());
        ofstream adSlotImageTempFile;
        adSlotImageTempFile.open(tempFilenameSoNotClashingWithNeighborSessions_AndAlsoForExistenceAtomicity.c_str());
        adSlotImageTempFile << base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_IMAGEB64));
        adSlotImageTempFile.close();
        rename(tempFilenameSoNotClashingWithNeighborSessions_AndAlsoForExistenceAtomicity.c_str(), permaCacheFilePath.c_str()); //TODOreq: thread safe? two sessions might call it near simultaneously. rename needs to be atomic. so long as the results aren't intertwined or some such (mangled inode data? idfk. i really doubt it's a problem, and would lol at linus torvalds if it was), it doesn't matter which of the two succeed
    }

    //TODOoptional: if efficiency isn't priority but saving hdd space is, can use the file resource that deletes the underlying file on destruction instead
    //TODOoptimization: custom wfileresource with 1 year expiration date
    WFileResource *adImageResource = new WFileResource("image/" + guessedExtensionAndMimeType.second, permaCacheFilePath, adImageAnchorOrderingPlaceholderContainer);
    adImageResource->suggestFileName("image" + guessedExtensionAndMimeType.first, WResource::Inline);
    const std::string &adImageHoverText = base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_HOVERTEXT));
    WImage *adImage = new WImage(adImageResource, adImageHoverText);
    adImage->resize(ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS, ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS);
    WAnchor *adImageAnchor = new WAnchor(WLink(WLink::Url, base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_URL))), adImage, adImageAnchorOrderingPlaceholderContainer);
    adImageAnchor->setTarget(TargetNewWindow);
    adImage->setToolTip(adImageHoverText);
    adImageAnchor->setToolTip(adImageHoverText);

    if(m_NumberOfAdsHackilyMultiGetting == 0)
    {
        //got all (just got last) ad slot fillers for this page
        m_AllAdSlotFillersPagesStack->setCurrentWidget(m_PointerToCurrentPageInConstruction);
        m_AdSlotFillersPagesOneBasedIndexHash[m_CurrentPageOneIndexBased] = m_PointerToCurrentPageInConstruction; //record the fact that the page is created, so we don't re-create it if they come back to this page
        m_AbcApp->resumeRendering();
    }
}
void ViewAllExistingAdSlotFillersAccountTabBody::firstPageButtonClicked()
{
    if(m_CurrentPageOneIndexBased == 1)
    {
        //should never get here, but just in case we do (bug), we make sure it won't happen again xD
        m_FirstPageButton->disable();
        m_PreviousPageButton->disable();
        return;
    }
    m_CurrentPageOneIndexBased = 1;
    doAllAdSlotFillersViewPageChange();
}
void ViewAllExistingAdSlotFillersAccountTabBody::previousPageButtonClicked()
{
    if(m_CurrentPageOneIndexBased == 1)
    {
        //should never get here, but just in case we do (bug), we make sure it won't happen again xD
        m_FirstPageButton->disable();
        m_PreviousPageButton->disable();
        return;
    }
    --m_CurrentPageOneIndexBased;
    doAllAdSlotFillersViewPageChange();
}
void ViewAllExistingAdSlotFillersAccountTabBody::goToPageButtonClicked()
{
    if(m_PageSpinbox->validate() != WValidator::Valid)
    {
        //uhh.....
        return;
    }
    const int newPage = m_PageSpinbox->value();
    if(newPage == m_CurrentPageOneIndexBased)
    {
        //change to same page? bah. TODOoptional: if js is enabled, listen to 'changed' and then enable/disable it if not current page (still do this sanity check though of course). would start off disabled in that case...
        return;
    }
    m_CurrentPageOneIndexBased = newPage;
    doAllAdSlotFillersViewPageChange();
}
void ViewAllExistingAdSlotFillersAccountTabBody::nextPageButtonClicked()
{
    if(m_CurrentPageOneIndexBased == m_TotalPagesCount)
    {
        //should never get here, but just in case we do (bug), we make sure it won't happen again xD
        m_NextPageButton->disable();
        m_LastPageButton->disable();
        return;
    }
    ++m_CurrentPageOneIndexBased;
    doAllAdSlotFillersViewPageChange();
}
void ViewAllExistingAdSlotFillersAccountTabBody::lastPageButtonClicked()
{
    if(m_CurrentPageOneIndexBased == m_TotalPagesCount)
    {
        //should never get here, but just in case we do (bug), we make sure it won't happen again xD
        m_NextPageButton->disable();
        m_LastPageButton->disable();
        return;
    }
    m_CurrentPageOneIndexBased = m_TotalPagesCount;
    doAllAdSlotFillersViewPageChange();
}
void ViewAllExistingAdSlotFillersAccountTabBody::doAllAdSlotFillersViewPageChange()
{
    //Enable/Disable Previous/Next buttons depending...
    m_FirstPageButton->setDisabled(m_CurrentPageOneIndexBased == 1);
    m_PreviousPageButton->setDisabled(m_CurrentPageOneIndexBased == 1);
    m_PageSpinbox->setValue(m_CurrentPageOneIndexBased);
    m_NextPageButton->setDisabled(m_CurrentPageOneIndexBased == m_TotalPagesCount);
    m_LastPageButton->setDisabled(m_CurrentPageOneIndexBased == m_TotalPagesCount);

    //first see if the page is already made. if it is, we just bring it to the top of the stacked widget
    try
    {
        WContainerWidget *pageAlreadyInStackedWidgetMaybe = m_AdSlotFillersPagesOneBasedIndexHash.at(m_CurrentPageOneIndexBased);

        //no exception = page already created
        m_AllAdSlotFillersPagesStack->setCurrentWidget(pageAlreadyInStackedWidgetMaybe);
    }
    catch(std::out_of_range &pageNotVisitedYetException)
    {
        //page not in hash/stack yet, so make it and add it
        buildCurrentPageAndAddToStackAndPageHash();
        m_AbcApp->deferRendering();
    }
}
