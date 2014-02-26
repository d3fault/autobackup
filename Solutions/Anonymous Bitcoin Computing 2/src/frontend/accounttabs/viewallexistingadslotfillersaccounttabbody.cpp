#include "viewallexistingadslotfillersaccounttabbody.h"

#include "../anonymousbitcoincomputingwtgui.h"

#include "abc2couchbaseandjsonkeydefines.h"

#define VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE 10

//all pages are 1 index based, even though the db stuff backing it is not

//TODOoptimization: for js, we should triggerUpdate the images as they come. no-js just stays deferRendered until the last one...
ViewAllExistingAdSlotFillersAccountTabBody::ViewAllExistingAdSlotFillersAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp), m_CurrentPageOneIndexBased(1)
{ }
void ViewAllExistingAdSlotFillersAccountTabBody::populateAndInitialize()
{
    m_AbcApp->getCouchbaseDocumentByKeyBegin(adSpaceAllSlotFillersKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETALLADSLOTFILLERSDOCFORVIEWING;

    //TODOoptimization: re-organize other methods all throughout app so that the db hit is scheduled/in-progress before doing any other stuff (that the db hit doesn't depend on obviously), like for example the below object instantiations and call to deferRendering...
    m_AbcApp->deferRendering();
    new WText("View all your previously uploaded advertisements", this);
    new WBreak(this);
    //TODOreq: refresh buttan to pull in new uploads during the same session (low priority, since they already have a preview after upload..), OR uploading one can 'push' it directly over to us (after checking we aren't NULL or something). That pushing over strategy wouldn't work if they have two tabs/sessions open, but oh well

    //TODOreq: showing them total pages and including a 'jump to page' spinbox would be easy, but note that if you do that then the 'index' in stackedwidget won't correlate with the actual page, so if they hit 'previous page' after jumping to... say... 100, they should then go to page 99 and not whatever page they jumped from (page 1 for example), despite page 100 being 'index 1' and page 1 being 'index 0'
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

    m_AdsCount = boost::lexical_cast<int>(pt.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT));
    m_TotalPagesCount = static_cast<int>(ceil(static_cast<double>(m_AdsCount) / static_cast<double>(VIEW_ALL_AD_SLOT_FILLERS_TAB_NUM_AD_SLOT_FILLERS_PER_PAGE)));

    //[First-button][Previous-button] Page [spinbox-showing-current-page] [Go-button] of %numPages% [Next-button][Last-button] ...... where First/Previous/Next/Last are enabled/disabled as appropriate

    m_FirstPageButton = new WPushButton("First Page", this);
    m_FirstPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::firstPageButtonClicked);
    m_FirstPageButton->disable(); //starts disabled because on first page

    m_PreviousPageButton = new WPushButton("Previous Page", this);
    m_PreviousPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::previousPageButtonClicked);
    m_PreviousPageButton->disable(); //ditto

    new WText(" Page: ", this);

    m_PageSpinbox = new WSpinBox(this); //TODOreq: can't set validator because built in, but does C++ still need to call validate() ?
    m_PageSpinbox->setRange(1, m_TotalPagesCount);
    m_PageSpinbox->setValue(1);
    m_PageSpinbox->enterPressed().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::goToPageButtonClicked);

    WPushButton *goToPageButton = new WPushButton("Go", this); //TODOreq: handle jumping to same page by doing nothing
    goToPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::goToPageButtonClicked);

    new WText(" of " + boost::lexical_cast<std::string>(m_TotalPagesCount) + " pages. ", this);

    m_NextPageButton = new WPushButton("Next Page", this);
    m_NextPageButton->clicked().connect(this, &ViewAllExistingAdSlotFillersAccountTabBody::nextPageButtonClicked);
    m_NextPageButton->setEnabled(m_TotalPagesCount > 1);

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
        m_AbcApp->getCouchbaseDocumentByKeyBegin(keyToCurrentAdSlotFiller);
        m_AdSlotFillersOnePageOrderingHash[keyToCurrentAdSlotFiller] = new WContainerWidget(m_PointerToCurrentPageInConstruction); //empty placeholder, setting up the ordering here/now. the hash is to be able to access it again later to populate it
    }
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::HACKYMULTIGETAPAGEWORTHOFADSLOTFILLERS; //came up with this while sleeping, should work...
}
void ViewAllExistingAdSlotFillersAccountTabBody::oneAdSlotFillerFromHackyMultiGetAttemptFinished(const std::string &keyToAdSlotFillerArriving, const string &oneAdSlotFillerJsonDoc, bool lcbOpSuccess, bool dbError)
{
    //TODOreq: ordering of the responses (key analysis? idfk)
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

    SingleUseSelfDeletingMemoryResource *adImageResource = new SingleUseSelfDeletingMemoryResource(base64Decode(pt.get<std::string>(JSON_SLOT_FILLER_IMAGEB64)), adImageAnchorOrderingPlaceholderContainer);
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
    m_PageSpinbox->value();
    if(m_PageSpinbox->value() == m_CurrentPageOneIndexBased)
    {
        //change to same page? bah. TODOoptional: if js is enabled, listen to 'changed' and then enable/disable it if not current page (still do this sanity check though of course). would start off disabled in that case...
        return;
    }
    m_CurrentPageOneIndexBased = m_PageSpinbox->value();
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
