#ifndef ADCAPTCHASITE_H
#define ADCAPTCHASITE_H

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/Auth/AuthWidget>
using namespace Wt;

#include "owners/campaigneditorview.h"
#include "publishers/adeditorview.h"
#include "adcaptchasitehome.h"

#include "../database.h"

#define HOME_PATH "/home"
#define OWNER_PATH "/owner"
#define PUBLISHER_PATH "/publisher"

class AdCaptchaSiteHome;
class PublisherCampaignsView;
class OwnerCampaignsView;

class AdCaptchaSite : public WContainerWidget
{
public:
    AdCaptchaSite(WContainerWidget *parent = 0);

    static std::string getHomePath();
    static std::string getOwnerPath();
    static std::string getPublisherPath();

private:
    WStackedWidget *m_ViewStack;

    AdCaptchaSiteHome *m_HomeView;
    OwnerCampaignsView *m_CampaignEditorView; //for site owners
    PublisherCampaignsView *m_AdEditorView; //for ad publishers
    //are there more?

    Database m_MyDb;

    void handleLoginChanged();
    void handleInternalPathChanged(const std::string &newInternalPath);

    void showHome();
    void showOwner();
    void showPublisher();
};

#endif // ADCAPTCHASITE_H
