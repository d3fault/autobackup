#ifndef ADVERTISINGBUYADSPACEALLUSERSWITHATLEASTONEADCAMPAIGNWIDGET_H
#define ADVERTISINGBUYADSPACEALLUSERSWITHATLEASTONEADCAMPAIGNWIDGET_H

#include <Wt/WContainerWidget>
using namespace Wt;

#include "../../abc2common.h"

class AnonymousBitcoinComputingWtGUI;

class AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget : public WContainerWidget
{
public:
    AdvertisingBuyAdSpaceAllUsersWithAtLeastOneAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent = 0);
private:
    friend class AnonymousBitcoinComputingWtGUI;
    AnonymousBitcoinComputingWtGUI *m_AnonymousBitcoinComputingWtGUI;

    void showPageOfUsersWithAtLeastOneAdCampaign(const ViewQueryPageContentsType &pageContents, bool internalServerErrorOrJsonError);
};

#endif // ADVERTISINGBUYADSPACEALLUSERSWITHATLEASTONEADCAMPAIGNWIDGET_H
