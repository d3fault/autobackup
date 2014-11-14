#include "advertisingbuyownersadspacecampaignwithindexwidget.h"

#define AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget_MID_TITLE_TEXT "'s Ad Campaign #"

//TODOreq: navigating to a campaign index that does not exist should present a better error. Right now it shows an error message below a bunch of EMPTY fields that shouldn't exist (note, don't delete them with 'clear()' though, that's dangerous :-/ I just found out the hard way heh. If they later choose an index that is valid, we'll try to update deleted placeholder pointers and blamo segfault)
//even though i know i can/should refactor tons of code into here, i'm only going to do it when necessary for the multi campaign owners refactor. ideally, all the buy code would go in here (and perhaps even more sub-classes). i'm also not ifdef'ing this out for the multi owner refactor because the functionality should be identical in the end (and it would be way too many ifdefs)
AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget::AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget(const std::string &campaignOwner, const std::string &campaignIndex, WContainerWidget *parent)
    : WContainerWidget(parent)
{
    setOverflow(WContainerWidget::OverflowAuto);

    m_CampaignOwnerAndIndexTitlePlaceholder = new WText(campaignOwner + AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget_MID_TITLE_TEXT + campaignIndex, this);
}
void AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget::setCampaignOwnerAndCampaignIndex(const std::string &campaignOwner, const std::string &campaignIndex)
{
    m_CampaignOwnerAndIndexTitlePlaceholder->setText(campaignOwner + AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget_MID_TITLE_TEXT + campaignIndex);
}
