#ifndef ADCAPTCHASITE_H
#define ADCAPTCHASITE_H

#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
using namespace Wt;

#include "owners/campaigneditorview.h"
#include "publishers/adeditorview.h"

#include "../database.h"

class AdCaptchaSite : public WContainerWidget
{
public:
    AdCaptchaSite(WContainerWidget *parent = 0);
private:
    WStackedWidget *m_ViewStack;

    CampaignEditorView *m_CampaignEditorView; //for site owners
    AdEditorView *m_AdEditorView; //for ad publishers
    //are there more?

    Database m_MyDb;

    void handleLoginChanged();
};

#endif // ADCAPTCHASITE_H
