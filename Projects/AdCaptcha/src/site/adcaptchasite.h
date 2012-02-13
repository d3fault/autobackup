#ifndef ADCAPTCHASITE_H
#define ADCAPTCHASITE_H

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/Auth/AuthWidget>
#include <Wt/WAnchor>
#include <Wt/WBreak>
using namespace Wt;

#include "owners/campaigneditorview.h"
#include "publishers/adeditorview.h"

#include "../database.h"

#define HOME_PATH "/home"
#define OWNER_PATH "/owner"
#define PUBLISHER_PATH "/publisher"

class AdCaptchaSite : public WContainerWidget
{
public:
    AdCaptchaSite(WContainerWidget *parent = 0);

private:
    WStackedWidget *m_ViewStack;

    WContainerWidget *m_HomeView;
    CampaignEditorView *m_CampaignEditorView; //for site owners
    AdEditorView *m_AdEditorView; //for ad publishers
    //are there more?

    Database m_MyDb;

    void handleLoginChanged();
    void handleInternalPathChanged(const std::string &newInternalPath);

    void showHome();
    void showOwner();
    void showPublisher();
};

#endif // ADCAPTCHASITE_H
