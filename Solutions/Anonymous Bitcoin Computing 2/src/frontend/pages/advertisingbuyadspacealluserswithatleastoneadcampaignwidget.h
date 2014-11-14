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

    int m_CurrentPage;
    int m_TotalPages;
    void setTotalPages(int totalPages);

    WPushButton *m_FirstPageButton;
    WPushButton *m_PreviousPageButton;
    WSpinBox *m_PageSpinBox;
    WPushButton *m_NextPageButton;
    WPushButton *m_LastPageButton;

    WContainerWidget *m_Contents;

    void beginGettingPage(int pageNum);
    void showPageOfUsersWithAtLeastOneAdCampaign(const ViewQueryPageContentsType &pageContents, int totalPages, bool internalServerErrorOrJsonError);

    void handleFirstPageButtonClicked();
    void handlePreviousPageButtonClicked();
    void handlePageSpinBoxGoButtonClicked();
    void handleNextPageButtonClicked();
    void handleLastPageButtonClicked();

    void enableOrDisablePageNavigationButtonsBasedOnCurrentPageAndTotalPages();
};

#endif // ADVERTISINGBUYADSPACEALLUSERSWITHATLEASTONEADCAMPAIGNWIDGET_H
