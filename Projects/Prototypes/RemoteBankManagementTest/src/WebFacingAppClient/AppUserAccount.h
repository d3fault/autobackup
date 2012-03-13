#ifndef APPUSERACCOUNT_H
#define APPUSERACCOUNT_H

#include <QList>

//each user has a many to one of ad owner ad campaigns, and of purchased ad slots. aside from that, i can't think of what else (in the impl there will be password, authtokens, etc)

struct SiteOwnerAdCampaign
{
    //the owner, the name of the campaign, maybe a minimum amount to sell a slot at...???
    QString CampaignName;
};
struct PurchasedAdSlot
{
    //the amount it was purchased for, who purchased it, the corresponding SiteOwnerAdCampaigns, the slot within
};
//note that the two above structs represent tables of their own that will be pointed to as foreign keys/many-to-one relationships or whatever in the AppUserAccount. so it might actually be redundant to store the owner inside SiteOwnerAdCampaigns, for example. i'm starting to think maybe i should quit prototyping and start mucking about with Wt. i think i will once i get this slot purchasing logic out of the way. but maybe it'll be easier to conceptuatlize with the Wt dbo abstraction (which i'm emulating here) actually here...
struct AppUserAccount
{
    QList<SiteOwnerAdCampaign*> SiteOwnerAdCampaignsList;
    QList<PurchasedAdSlot*> PurchasedAdSlotsList;
};

#endif // APPUSERACCOUNT_H
