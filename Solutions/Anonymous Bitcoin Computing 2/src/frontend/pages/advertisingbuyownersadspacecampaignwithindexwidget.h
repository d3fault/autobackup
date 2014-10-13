#ifndef ADVERTISINGBUYOWNERSADSPACECAMPAIGNWITHINDEXWIDGET_H
#define ADVERTISINGBUYOWNERSADSPACECAMPAIGNWITHINDEXWIDGET_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget : public WContainerWidget
{
public:
    AdvertisingBuyOwnersAdSpaceCampaignWithIndexWidget(const std::string &campaignOwner = std::string(), const std::string &campaignIndex = std::string(), WContainerWidget *parent = 0);
    void setCampaignOwnerAndCampaignIndex(const std::string &campaignOwner, const std::string &campaignIndex);
private:
    WText *m_CampaignOwnerAndIndexTitlePlaceholder;
};

#endif // ADVERTISINGBUYOWNERSADSPACECAMPAIGNWITHINDEXWIDGET_H
