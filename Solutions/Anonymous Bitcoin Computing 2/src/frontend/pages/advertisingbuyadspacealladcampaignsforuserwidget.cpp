#include "advertisingbuyadspacealladcampaignsforuserwidget.h"

#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WSpinBox>
#include <Wt/WPushButton>

#include "../anonymousbitcoincomputingwtgui.h"
#include "../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h"

AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, const std::string &campaignOwner, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_AnonymousBitcoinComputingWtGUI(anonymousBitcoinComputingWtGUI)
    , m_CampaignOwner(campaignOwner)
{
    //just a spinbox for now. in the future if/when ad campaigns become delete-able, we'll need to use a view and proper paginzation etc. and once I allow campaigns to have titles and such then views are a must :(

    new WText("User '", this);
    m_UsernamePlaceholder = new WText(campaignOwner, this);
    new WText("' has ", this);
    m_NumAdCampaignsPlaceholder = new WText("1", this);
    new WText(" advertising campaigns. Use the spinbox below to navigate to an advertising campaign. Please note that the campaign indexes start at 0.", this);
    new WBreak(this);
    new WBreak(this);
    m_AdCampaignNavigationSpinbox = new WSpinBox(this);
    WPushButton *goButton = new WPushButton("Go", this);

    m_AdCampaignNavigationSpinbox->setValue(0);
    m_AdCampaignNavigationSpinbox->setRange(0, 0);

    beginFiguringOutHowManyAdCampaignsUserHas();

    goButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::handleGoButtonClicked);
}
void AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::setCampaignOwnerWeWantToSeeAdCampaignsForAndBeginFiguringOutHowManyAdCampaignsUserHasIfTheUsernameActuallyChanged(const string &newUsername)
{
    if(newUsername != m_CampaignOwner)
    {
        m_CampaignOwner = newUsername;
        m_UsernamePlaceholder->setText(newUsername);
        beginFiguringOutHowManyAdCampaignsUserHas();
    }
}
void AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::beginFiguringOutHowManyAdCampaignsUserHas()
{
    //TODOreq: i'm considering making the "ad campaigns current index cache" 'authorative', which means it... errr... I'm not sure.... will be made sooner? Must always exist when (when? when the user is created it is created? when the first ad campaign is made? maybe I should merge it with profile itself? baw idfk). I should also note that I should probably add BETTER 'recovery' for the index cache doc. Right now if it fails idgaf. But if I want it to be authorative, we should 'investigate' shenanigans when a fail happens. NVM: A normal fail scenario that I before now didn't care about but as of now I do care about is "two tabs creating an ad campaign simultaneously" ... first tab creates ad campaign #3, second tab creates ad campaign #4, first tab gets the cache doc for CAS swapping (it's pointing at 2, second tab gets the -- wait no that means lasjflasjdf;asljf we'd enver had made #4 if wait no idfk head hurts confused fuck it for now KISS not error cases galore

    m_AnonymousBitcoinComputingWtGUI->getCouchbaseDocumentByKeyBegin(adSpaceCampaignIndexCacheKey(m_CampaignOwner));
    m_AnonymousBitcoinComputingWtGUI->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETADCAMPAIGNCURRENTINDEXCACHEDOCFORDETERMININGHOWMANYADCAMPAIGNSAUSERHAS;
}
void AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::readCampaignIndexCacheDocToFigureOutHowManyAdCampaignsTheCampaignOwnerHas(const string &campaignIndexCacheDoc, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        //clear();
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "readCampaignIndexCacheDocToFigureOutHowManyAdCampaignsTheCampaignOwnerHas db error" << endl;
        return;
    }
    if(!lcbOpSuccess)
    {
        //they might have navigated to the url: the user might exist but they might not have any ad campaigns. OR, the 'cache' (soon to be authorative) index doc may have simply failed to have been created (additionally, it may have failed to have been updated. idk when/where the best spot to do the recovery logic for that doc is :-/ TODOreq: need to find SOMEWHERE, especially if making it authoriative)
        //segfaults if they later navigate to a user with an ad campaign: clear();
        new WText("User " + m_CampaignOwner + " does not appear to have any advertising campaigns. If you know this is an error, please report it", this);
        return;
    }

    //getting here means the campaign index doc was retrieved
    ptree pt;
    std::istringstream is(campaignIndexCacheDoc);
    read_json(is, pt);

    const std::string &nextAvailableAdCampaignIndex = pt.get<std::string>(JSON_AD_SPACE_CAMPAIGN_NEXT_AVAILABLE_INDEX_CACHE);
    //the index is zero-based, but it's also pointing "one ahead" (since it's the NEXT AVAILABLE index), so we lucked out and don't have to add/subtract 1 to it: the 'next available index' is also the total amount of ad campaigns :-P
    m_NumAdCampaignsPlaceholder->setText(nextAvailableAdCampaignIndex);
    //aww shit we still need to convert to int for giving to the spinbox wrrrrrrryyyy
    m_AdCampaignNavigationSpinbox->setMaximum(boost::lexical_cast<int>(nextAvailableAdCampaignIndex)-1); //0-based index, so subtract 1
}
void AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget::handleGoButtonClicked()
{
    const std::string &campaignIndexToNavigateTo = boost::lexical_cast<std::string>(m_AdCampaignNavigationSpinbox->value()); //TODOreq: sanitize before trying? eh it'll 404 anyways
    m_AnonymousBitcoinComputingWtGUI->setInternalPath(ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE "/" + m_CampaignOwner + "/" + campaignIndexToNavigateTo, true);
}
