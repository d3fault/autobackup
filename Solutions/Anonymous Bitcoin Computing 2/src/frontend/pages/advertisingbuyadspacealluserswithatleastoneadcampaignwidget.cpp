#include "advertisingbuyadspacealluserswithatleastoneadcampaignwidget.h"

#include <Wt/WText>
#include <Wt/WBreak>

#include "../anonymousbitcoincomputingwtgui.h"

#define AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget_PAGE_GET_PARAM_KEY "page"

AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_AnonymousBitcoinComputingWtGUI(anonymousBitcoinComputingWtGUI)
{
    new WText("All Users with Advertising Campaigns", this);
    new WBreak(this);
    new WBreak(this);

    //TODOreq: the pagenum has to be a get (or post) param. can't be part of the clean URL, otherwise it'd clash with the campaign owner usernames
    //TODOreq: spinbox, first/previous/next/last
    //TODOmb: customizeable 'items per page'
    int pageToGet = 1;
    const std::string *pageParamMaybe = m_AnonymousBitcoinComputingWtGUI->environment().getParameter(AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget_PAGE_GET_PARAM_KEY);
    if(pageParamMaybe)
    {
        pageToGet = boost::lexical_cast<int>(*pageParamMaybe); //TODOreq: sanitize. but before checking >= 1, boost lexical casting to int would throw exception if not int... so... idfk yet lolol
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
        new WText("The requested page does not exist", this);
        return;
    }
    for(ViewQueryPageContentsType::const_iterator it = pageContents.begin(); it != pageContents.end(); ++it)
    {
        new WText(*it, this);
        new WBreak(this);
    }
}
