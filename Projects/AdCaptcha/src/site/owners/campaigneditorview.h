#ifndef CAMPAIGNEDITORVIEW_H
#define CAMPAIGNEDITORVIEW_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
using namespace Wt;

#include "../../database.h"

class CampaignEditorView : public WContainerWidget
{
public:
    CampaignEditorView(Database *dbInstance, WContainerWidget *parent = 0);
private:
    Database *m_MyDb;
};

#endif // CAMPAIGNEDITORVIEW_H
