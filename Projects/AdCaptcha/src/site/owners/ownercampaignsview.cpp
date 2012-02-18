#include "ownercampaignsview.h"

OwnerCampaignsView::OwnerCampaignsView(Database *dbInstance, WContainerWidget *parent)
    : WContainerWidget(parent),
      m_MyDb(dbInstance)
{
    new WText("Hello Owner Campaign Editor View LoL", this);
    new WBreak(this);
    new WText("lol i fucking love Wt/C++", this);
    new WBreak(this);
    WAnchor *publisherAnchor = new WAnchor(AdCaptchaSite::getPublisherPath(), "Publishers");
    publisherAnchor->setRefInternalPath(AdCaptchaSite::getPublisherPath());
}
