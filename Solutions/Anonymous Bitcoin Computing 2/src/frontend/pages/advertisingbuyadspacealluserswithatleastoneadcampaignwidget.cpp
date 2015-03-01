#include "advertisingbuyadspacealluserswithatleastoneadcampaignwidget.h"

#include <Wt/WPushButton>
#include <Wt/WSpinBox>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WIntValidator>

#include "../anonymousbitcoincomputingwtgui.h"

#define AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget_PAGE_GET_PARAM_KEY "page"

//TODOoptional: Wt processes (woot) but removes (wtf?) the ?page=X from the URL...... but only when javascript is disabled...
AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_AnonymousBitcoinComputingWtGUI(anonymousBitcoinComputingWtGUI)
    , m_CurrentPage(1)
    , m_TotalPages(1) //we don't know the actual value until after the first page (first page requested, not necessarily "page 1") is retrieved
{
    new WText("All Users with Ad Space For Sale", this);
    new WBreak(this);
    new WBreak(this);

    //spinbox, first/previous/next/last
    m_FirstPageButton = new WPushButton("First Page", this);
    m_PreviousPageButton = new WPushButton("Previous Page", this);
    m_PageSpinBox = new WSpinBox(this);
    WPushButton *pageSpinBoxGoButton = new WPushButton("Go", this); //TODOoptional: enable/disable depending on if the value of the spinbox is within range. just saves us a trip to backend is all. only works with js enabled ofc
    m_NextPageButton = new WPushButton("Next Page", this);
    m_LastPageButton = new WPushButton("Last Page", this);

    new WBreak(this);
    new WBreak(this);

    m_Contents = new WContainerWidget(this);

    m_FirstPageButton->setDisabled(true);
    m_PreviousPageButton->setDisabled(true);
    m_PageSpinBox->setValue(1);
    m_PageSpinBox->setRange(1, m_TotalPages);
    m_NextPageButton->setDisabled(true);
    m_LastPageButton->setDisabled(true);

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
    beginGettingPage(pageToGet);

    m_FirstPageButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleFirstPageButtonClicked);
    m_PreviousPageButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handlePreviousPageButtonClicked);
    pageSpinBoxGoButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handlePageSpinBoxGoButtonClicked);
    m_NextPageButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleNextPageButtonClicked);
    m_LastPageButton->clicked().connect(this, &AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleLastPageButtonClicked);
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::setTotalPages(int totalPages)
{
    if(totalPages != m_TotalPages)
    {
        m_TotalPages = totalPages;
        m_PageSpinBox->setMaximum(totalPages);
    }
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::beginGettingPage(int pageNum)
{
    m_CurrentPage = max(1, pageNum); //lol lazy sanitizing ftw (backend can't handle < 1). TODOoptional: should wait until backend confirms that the page exists, but fuck it lewl lazy
    m_AnonymousBitcoinComputingWtGUI->queryCouchbaseViewBegin("_design/AllUsersWithAtLeastOneAdCampaign/_view/AllUsersWithAtLeastOneAdCampaign", pageNum);
    m_AnonymousBitcoinComputingWtGUI->m_WhatTheQueryCouchbaseViewWasFor = AnonymousBitcoinComputingWtGUI::ALLUSERSWITHATLEASTONEADCAMPAIGNQUERYCOUCHBASEVIEW;
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::showPageOfUsersWithAtLeastOneAdCampaign(const ViewQueryPageContentsType &pageContents, int totalPages, bool internalServerErrorOrJsonError)
{
    m_Contents->clear();
    if(internalServerErrorOrJsonError)
    {
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, m_Contents);
        return;
    }
    if(pageContents.get()->empty())
    {
        new WText("The requested page does not exist", m_Contents); //TODOmb: a link back to page 1. clicking "buy ad space" does not bring us back to page 1 either, but being on an invalid page is not very common anyways...
        return;
    }

    setTotalPages(totalPages);
    enableOrDisablePageNavigationButtonsBasedOnCurrentPageAndTotalPages();

    for(ViewQueryPageContentsPointedType::const_iterator it = pageContents.get()->begin(); it != pageContents.get()->end(); ++it)
    {
        const std::string &username = *it;
        new WAnchor(WLink(WLink::InternalPath, ABC_INTERNAL_PATH_ADS_BUY_AD_SPACE "/" + username), username, m_Contents);
        new WBreak(m_Contents);
    }
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleFirstPageButtonClicked()
{
    beginGettingPage(1);
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handlePreviousPageButtonClicked()
{
    beginGettingPage(m_CurrentPage-1);
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handlePageSpinBoxGoButtonClicked()
{
    //TODOreq: append ?page=blah to url, but don't emit signal for it
    //TODOreq: sanitize? fucking Wt docs don't make it clear. And shit my backend does pretty smart sanitizing anyways
    beginGettingPage(m_PageSpinBox->value());
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleNextPageButtonClicked()
{
    beginGettingPage(m_CurrentPage+1);
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::handleLastPageButtonClicked()
{
    beginGettingPage(m_TotalPages);
}
void AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget::enableOrDisablePageNavigationButtonsBasedOnCurrentPageAndTotalPages()
{
    if(m_CurrentPage < 2)
    {
        //first page
        m_FirstPageButton->setDisabled(true);
        m_PreviousPageButton->setDisabled(true);
    }
    else
    {
        //not first page
        m_FirstPageButton->setDisabled(false);
        m_PreviousPageButton->setDisabled(false);
    }

    if(m_CurrentPage >= m_TotalPages)
    {
        //last page
        m_NextPageButton->setDisabled(true);
        m_LastPageButton->setDisabled(true);
    }
    else
    {
        //not last page
        m_NextPageButton->setDisabled(false);
        m_LastPageButton->setDisabled(false);
    }
}
