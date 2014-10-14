#include "advertisingselladspacecreatenewadcampaignwidget.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost::property_tree;

#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WBreak>
#include <Wt/WDoubleValidator>
#include <Wt/WIntValidator>
#include <Wt/WPushButton>

#include "../anonymousbitcoincomputingwtgui.h"
#include "../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h"

AdvertisingSellAdSpaceCreateNewAdCampaignWidget::AdvertisingSellAdSpaceCreateNewAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_AnonymousBitcoinComputingWtGUI(anonymousBitcoinComputingWtGUI)
    , m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist(0)
    , m_CampaignIndexToTryLcbAddingAt("0")
{
    /*WText *algorithmExplainationLabel = */new WText("Selling ad space works like this: you choose a minimum price to sell your ad space for, in addition to the duration their ad will be shown on your site for. As soon as somebody purchases your ad space, the current price DOUBLES from what it was at (minimum price in the beginning), and then gradually (linearly) decreases back down to your minimum price over the course of whatever duration you choose. The math works out so that as soon as their ad expires, the price is right back at your minimum price. As if that wasn't confusing enough, the price doubles EACH AND EVERY TIME your ad space is purchased (and just like before, it always reaches your minimum price at the exact moment when the last queued ad expires). This algorithm allows you to choose a low minimum price (so that your ad space is almost always sold) while still raising the price each time it's sold so that you make a decent amount of money. It's dynamic and finds your sweet spot with zero effort on your part. The purchasers of your ad space end up competing with each other to buy your ad space. You don't have to lift a finger for 'negotiation', it's completely automated.", Wt::PlainText, this); //TODOreq: wordy. TODOoptional: perhaps hide behind a "CLICK HERE FOR HELP" thingo

    new WBreak(this);
    new WBreak(this);
    new WBreak(this);

    //TODOreq: minPrice and slot length hours (fill in with defaults (1 penny and 24 hours), and sanitize ofc (no lower than min bitcoin transaction amount, no lower than 1 hour)). it makes sense now to also include a url the ad will be shown on, and as mentioned earlier a human readable name...
    /*WText *minPriceLabel = */new WText("Minimum selling price, in Bitcoins:", Wt::PlainText, this);
    new WBreak(this);
    m_MinPriceLineEdit = new WLineEdit("0.001", this); //TODOmb: wdoublespinbox?
    WDoubleValidator *minPriceValidator = new WDoubleValidator(0.0001 /*TODOreq: what is current bitcoin minimum?*/, 21000000, m_MinPriceLineEdit);
    minPriceValidator->setMandatory(true);
    m_MinPriceLineEdit->setValidator(minPriceValidator);

    new WBreak(this);
    new WBreak(this);

    /*WText *slotLengthHoursLabel = */new WText("Duration of advertisement placement, in hours:", this);
    new WBreak(this);
    m_SlotLengthHoursLineEdit = new WLineEdit("24", this); //TODOmb: wintspinbox?
    WIntValidator *slotLengthHoursValidator = new WIntValidator(1, INT_MAX, m_SlotLengthHoursLineEdit); //TODOoptional: we could probably go higher than INT_MAX, since it's stored and a string and we use doubles for the math :-P. But wtf seriously, 245146 years is plenty. Maybe I should even lower this limit?
    slotLengthHoursValidator->setMandatory(true);
    m_SlotLengthHoursLineEdit->setValidator(slotLengthHoursValidator);

    new WBreak(this);
    new WBreak(this);

    WPushButton *createNewAdCampaignButton = new WPushButton("Create Ad Campaign", this);
    createNewAdCampaignButton->clicked().connect(this, &AdvertisingSellAdSpaceCreateNewAdCampaignWidget::createNewAdCampaignButtonClicked);
    createNewAdCampaignButton->clicked().connect(createNewAdCampaignButton, &WPushButton::disable);
}
void AdvertisingSellAdSpaceCreateNewAdCampaignWidget::createNewAdCampaignButtonClicked()
{
    //TODOreq: i'm still not sure whether or not I need to validate/sanitize again here/now or not. I recall reading that the "validators attached to inputs" are front-end only, but I could be wrong. for now taking the maybe-less-efficient safer route
    if(m_MinPriceLineEdit->validate() != WDoubleValidator::Valid)
    {
        new WBreak(this);
        new WText("Invalid minimum price", this); //TODOoptional: elaborate. however the js front-end of the validator will already do it in most cases
        return;
    }
    if(m_SlotLengthHoursLineEdit->validate() != WIntValidator::Valid)
    {
        new WBreak(this);
        new WText("Invalid advertisement duration", this);
        return;
    }

    //TODOreq: do i need an "all campaigns doc" just like my allSlotFillers doc? I definitely need to figure out which campaign index to put it into, but I could use a "cache" strategy for that instead. allSlotFillers was required for that qcombobox (although a view (map) could do it in cb 3.0 fml), so maybe "allcampaignsdoc" isn't necessary? it definitely isn't NECESSARY if I either use a "cache" doc... or if I upgrade to cb 3.0 and use views.
    //Regardless, TODOreq I want to LCB_ADD at the first available campaign index (keep +1'ing if the LCB_ADD op fails). And well shit, I think that's.... IT... wowowow ezmode (compared to the slot filling code)...

    //For now I'm just going to do a not-relied-upon "cache" doc that says which index to add to. I luld when trying to decide whether or not to make it "authorative with recovery" or "not-relied-upon cache" and realizing that those are both the same fucking thing!
    //I might change the design later, but who cares for now. ALSO simply knowing HOW MANY campaigns there are allows me to do a minimal "list of campaigns" page (rather, series of pages). If/when I start referring to them by human readable names though, then more db access/docs is required

    m_AnonymousBitcoinComputingWtGUI->deferRendering(); //TODOreq: resume later (and at fail points). the reason i defer is so i know the line edits aren't tampered with and i can save myself from having to store their texts as additional member varaibles

    m_AnonymousBitcoinComputingWtGUI->getCouchbaseDocumentByKeySavingCasBegin(adSpaceCampaignIndexCacheKey(m_AnonymousBitcoinComputingWtGUI->m_CurrentlyLoggedInUsername));
    m_AnonymousBitcoinComputingWtGUI->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETADSPACECAMPAIGNINDEXCACHEEVENTHOUGHITMIGHTNOTEXIST;
}
void AdvertisingSellAdSpaceCreateNewAdCampaignWidget::useCacheToDetermineIndexForCreatingNewCampaignAtOrWalkThemUntilEmptyIndexFoundIfTheCacheDoesntExist(const string &couchbaseDocument, u_int64_t casForUpdatingTheCacheAfterWeCreateNewCampaign, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "useCacheToDetermineIndexForCreatingNewCampaignAtOrWalkThemUntilEmptyIndexFoundIfTheCacheDoesntExist db error" << endl;
        m_AnonymousBitcoinComputingWtGUI->resumeRendering();
        return;
    }

    m_CampaignIndexToTryLcbAddingAt = "0";
    if(lcbOpSuccess)
    {
        //parse what the cache indicates is the next available index, but DO NOT RELY ON IT TODOreq
        ptree pt;
        std::istringstream is(couchbaseDocument);
        read_json(is, pt);

        m_CampaignIndexToTryLcbAddingAt = pt.get<std::string>(JSON_AD_SPACE_CAMPAIGN_NEXT_AVAILABLE_INDEX_CACHE);
        m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist = casForUpdatingTheCacheAfterWeCreateNewCampaign;
    }
    else //cache doc doesn't exist
    {
        //walk the indexes until spot found. TODOoptimization, maybe use a better algorithm than "+1". Like double at each step and then half when you overshoot etc. TODOreq: worth noting that even if you find a spot (the get lcb op fails), it still might be taken by the time the LCB_ADD is performed (concurrent tabs etc). Hell come to think of it, fuck the get, just keep doing LCB_ADD over and over yeeeee. TODOoptional: the slot index cache equivalent (I think in "get todays ad slot") to this code might use GETs when as I've just realized, could skip that and just do LCB_ADDs

        m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist = 0;
    }

    //create campaign doc
    ptree pt;

    pt.put(JSON_AD_SPACE_CAMPAIGN_MIN_PRICE, m_MinPriceLineEdit->text().toUTF8());
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS, m_SlotLengthHoursLineEdit->text().toUTF8());

    std::ostringstream campaignDocBuffer;
    write_json(campaignDocBuffer, pt, false);

    m_CampaignDocForCampaignBeingCreated = campaignDocBuffer.str();

    attemptToCreateCampaign();
}
void AdvertisingSellAdSpaceCreateNewAdCampaignWidget::attemptToCreateCampaign()
{
    int rangeCheck = boost::lexical_cast<int>(m_CampaignIndexToTryLcbAddingAt);
    if(rangeCheck == INT_MAX)
    {
        new WBreak(this);
        new WText("You've reached the maximum amount of campaigns. You can always create another user.", this);
        m_AnonymousBitcoinComputingWtGUI->resumeRendering();
        return;
    }
    m_AnonymousBitcoinComputingWtGUI->store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(adSpaceCampaignKey(m_AnonymousBitcoinComputingWtGUI->m_CurrentlyLoggedInUsername, m_CampaignIndexToTryLcbAddingAt), m_CampaignDocForCampaignBeingCreated);
    m_AnonymousBitcoinComputingWtGUI->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::ATTEMPTTOADDCAMPAIGNATINDEX;
}
void AdvertisingSellAdSpaceCreateNewAdCampaignWidget::handleAttemptToAddCampaignAtIndexFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "handleAttemptToAddCampaignAtIndexFinished db error" << endl;
        m_AnonymousBitcoinComputingWtGUI->resumeRendering();
        return;
    }

    if(!lcbOpSuccess)
    {
        //+1 the index and try again
        m_CampaignIndexToTryLcbAddingAt = boost::lexical_cast<std::string>((boost::lexical_cast<int>(m_CampaignIndexToTryLcbAddingAt)+1));
        attemptToCreateCampaign();
        return;
    }

    //getting here means the campaign was created, so now we update the cache to point to +1 whatever we just got
    //TODOreq: if cas of cache is 0, do lcb_add [ignoring fail]. else, do lcb_set with cas [ignoring fail]
    std::string nextAvailableIndexToPutInCacheDoc = boost::lexical_cast<std::string>((boost::lexical_cast<int>(m_CampaignIndexToTryLcbAddingAt)+1));;

    ptree pt;
    pt.put(JSON_AD_SPACE_CAMPAIGN_NEXT_AVAILABLE_INDEX_CACHE, nextAvailableIndexToPutInCacheDoc);

    std::ostringstream campaignIndexCacheDocBuffer;
    write_json(campaignIndexCacheDocBuffer, pt, false);

    const std::string &campaignIndexCacheKey = adSpaceCampaignIndexCacheKey(m_AnonymousBitcoinComputingWtGUI->m_CurrentlyLoggedInUsername);
    if(m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist == 0)
    {
        m_AnonymousBitcoinComputingWtGUI->store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(campaignIndexCacheKey, campaignIndexCacheDocBuffer.str());
        m_AnonymousBitcoinComputingWtGUI->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::ADDCAMPAIGNINDEXCACHEIGNORINGRESPONSE;
    }
    else
    {
        m_AnonymousBitcoinComputingWtGUI->store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(campaignIndexCacheKey, campaignIndexCacheDocBuffer.str(), m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
        m_AnonymousBitcoinComputingWtGUI->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::CASSWAPCAMPAIGNINDEXCACHEIGNORINGRESPONSE;
    }

    new WBreak(this);
    new WText("Campaign at index #" + m_CampaignIndexToTryLcbAddingAt + " was successfully created");
    //hmm actually might as well leave whatever they entered... m_MinPriceLineEdit. if there was a human readable line edit though, we'd want to clear that one
    m_AnonymousBitcoinComputingWtGUI->resumeRendering();
}
