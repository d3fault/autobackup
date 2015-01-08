#ifndef ADVERTISINGSELLADSPACECREATENEWADCAMPAIGNWIDGET_H
#define ADVERTISINGSELLADSPACECREATENEWADCAMPAIGNWIDGET_H

#include <Wt/WContainerWidget>
using namespace Wt;

class AnonymousBitcoinComputingWtGUI;

class AdvertisingSellAdSpaceCreateNewAdCampaignWidget : public WContainerWidget
{
public:
    AdvertisingSellAdSpaceCreateNewAdCampaignWidget(AnonymousBitcoinComputingWtGUI *anonymousBitcoinComputingWtGUI, WContainerWidget *parent = 0);
private:
    friend class AnonymousBitcoinComputingWtGUI;
    AnonymousBitcoinComputingWtGUI *m_AnonymousBitcoinComputingWtGUI;

    WLineEdit *m_MinPriceLineEdit;
    WLineEdit *m_SlotLengthHoursLineEdit;

    WCheckBox *m_DisallowPornCheckbox;
    WCheckBox *m_DisallowGamblingCheckbox;
    WCheckBox *m_DisallowMedicineCheckbox;
    WCheckBox *m_DisallowAlcoholCheckbox;
    WCheckBox *m_DisallowTobaccoCheckbox;
    WCheckBox *m_DisallowWeaponsCheckbox;

    WPushButton *m_CreateNewAdCampaignButton;

    u_int64_t m_CasOfCampaignIndexCacheDoc_OrZeroIfCacheDocDoesNotExist;
    std::string m_CampaignIndexToTryLcbAddingAt;
    std::string m_CampaignDocForCampaignBeingCreated;

    void createNewAdCampaignButtonClicked();
    void useCacheToDetermineIndexForCreatingNewCampaignAtOrWalkThemUntilEmptyIndexFoundIfTheCacheDoesntExist(const std::string &couchbaseDocument, u_int64_t casForUpdatingTheCacheAfterWeCreateNewCampaign, bool lcbOpSuccess, bool dbError);
    void attemptToLcbAddCampaign();
    void handleAttemptToAddCampaignAtIndexFinished(bool lcbOpSuccess, bool dbError);
};

#endif // ADVERTISINGSELLADSPACECREATENEWADCAMPAIGNWIDGET_H
