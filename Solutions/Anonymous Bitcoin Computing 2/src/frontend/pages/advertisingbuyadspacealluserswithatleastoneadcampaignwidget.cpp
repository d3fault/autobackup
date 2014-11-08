#include "advertisingbuyadspacealluserswithatleastoneadcampaignwidget.h"

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WIntValidator>

#include "../anonymousbitcoincomputingwtgui.h"

#define AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget_PAGE_GET_PARAM_KEY "page"

//TODOoptional: Wt processes (woot) but removes (wtf?) the ?page=X from the URL...... but only when javascript is disabled...
AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_AnonymousBitcoinComputingWtGUI(anonymousBitcoinComputingWtGUI)
{
    new WText("All Users with Advertising Campaigns", this);
    new WBreak(this);
    new WBreak(this);

    //TODOreq: spinbox, first/previous/next/last
    //TODOmb: customizeable 'items per page'
    int pageToGet = 1;
    const std::string *pageParamMaybe = m_AnonymousBitcoinComputingWtGUI->environment().getParameter(AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget_PAGE_GET_PARAM_KEY); //the pagenum has to be a get (or post) param. can't be part of the clean URL, otherwise it'd clash with the campaign owner usernames. I could solve this by using a different path, but won't for now
    if(pageParamMaybe)
    {
        WIntValidator pageNumSanitizer(1, INT_MAX);
        pageNumSanitizer.setMandatory(true); //page='' ? idk lol
        if(pageNumSanitizer.validate(*pageParamMaybe).state() == WIntValidator::Valid)
        {
            pageToGet = boost::lexical_cast<int>(*pageParamMaybe); //why the fuck doesn't the validator give me the shit!? noobs
        }
    }
    m_AnonymousBitcoinComputingWtGUI->queryCouchbaseViewBegin("_design/dev_AllUsersWithAtLeastOneAdCampaign/_view/AllUsersWithAtLeastOneAdCampaign", pageToGet);
    m_AnonymousBitcoinComputingWtGUI->m_WhatTheQueryCouchbaseViewWasFor = AnonymousBitcoinComputingWtGUI::ALLUSERSWITHATLEASTONEADCAMPAIGNQUERYCOUCHBASEVIEW;
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::showPageOfUsersWithAtLeastOneAdCampaign(const ViewQueryPageContentsType &pageContents, bool internalServerErrorOrJsonError)
{
    if(internalServerErrorOrJsonError)
    {
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        return;
    }
    if(pageContents.empty())
    {
        new WText("The requested page does not exist", this); //TODOmb: a link back to page 1. clicking "buy ad space" does not bring us back to page 1 either, but being on an invalid page is not very common anyways...
        return;
    }
    for(ViewQueryPageContentsType::const_iterator it = pageContents.begin(); it != pageContents.end(); ++it)
    {
        new WText(*it, this);
        new WBreak(this);
    }
}
