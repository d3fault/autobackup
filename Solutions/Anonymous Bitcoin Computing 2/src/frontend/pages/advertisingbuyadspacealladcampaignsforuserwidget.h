#ifndef ADVERTISINGBUYADSPACEALLADCAMPAIGNSFORUSERWIDGET_H
#define ADVERTISINGBUYADSPACEALLADCAMPAIGNSFORUSERWIDGET_H

#include <Wt/WContainerWidget>
using namespace Wt;

class AnonymousBitcoinComputingWtGUI;

class AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget : public WContainerWidget
{
public:
    AdvertisingBuyAdSpaceAllAdCampaignsForUserWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, const std::string &campaignOwner, WContainerWidget *parent = 0);
private:
    friend class AnonymousBitcoinComputingWtGUI;
    AnonymousBitcoinComputingWtGUI *m_AnonymousBitcoinComputingWtGUI;

    std::string m_CampaignOwner;

    WText *m_UsernamePlaceholder;
    WText *m_NumAdCampaignsPlaceholder;
    WSpinBox *m_AdCampaignNavigationSpinbox;

    void setCampaignOwnerWeWantToSeeAdCampaignsForAndBeginFiguringOutHowManyAdCampaignsUserHasIfTheUsernameActuallyChanged(const std::string &newUsername);

    void beginFiguringOutHowManyAdCampaignsUserHas();
    void readCampaignIndexCacheDocToFigureOutHowManyAdCampaignsTheCampaignOwnerHas(const std::string &campaignIndexCacheDoc, bool lcbOpSuccess, bool dbError);

    void handleGoButtonClicked();
};

#endif // ADVERTISINGBUYADSPACEALLADCAMPAIGNSFORUSERWIDGET_H
