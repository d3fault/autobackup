#ifndef OWNERCAMPAIGNSVIEW_H
#define OWNERCAMPAIGNSVIEW_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WAnchor>
using namespace Wt;

#include "../adcaptchasite.h"
#include "../../database.h"

class OwnerCampaignsView : public WContainerWidget
{
public:
    OwnerCampaignsView(Database *dbInstance, WContainerWidget *parent = 0);
private:
    Database *m_MyDb;
};

#endif // OWNERCAMPAIGNSVIEW_H
