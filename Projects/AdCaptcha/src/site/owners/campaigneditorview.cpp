#include "campaigneditorview.h"

CampaignEditorView::CampaignEditorView(Database *dbInstance, WContainerWidget *parent)
    : WContainerWidget(parent),
      m_MyDb(dbInstance)
{
    new WText("Hello Campaign Editor View LoL", this);
    new WBreak(this);
    new WText("lol i fucking love Wt/C++");
}
