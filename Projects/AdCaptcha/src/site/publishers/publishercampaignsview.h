#ifndef PUBLISHERCAMPAIGNSVIEW_H
#define PUBLISHERCAMPAIGNSVIEW_H

#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WAnchor>
using namespace Wt;

#include "../adcaptchasite.h"
#include "../../database.h"

class PublisherCampaignsView : public WContainerWidget
{
public:
    PublisherCampaignsView(Database *dbInstance, WContainerWidget *parent = 0);
private:
    Database *m_MyDb;
};

#endif // PUBLISHERCAMPAIGNSVIEW_H
