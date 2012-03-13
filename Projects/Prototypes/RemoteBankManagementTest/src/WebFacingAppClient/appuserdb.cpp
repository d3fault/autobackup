#include "appuserdb.h"

AppUserDb::AppUserDb(QObject *parent) :
    QObject(parent)
{
}
void AppUserDb::addUser(QString newUser)
{
    AppUserAccount *newUserAccount = new AppUserAccount();
    //init, except lol we have nothing to init in this prototype...
    m_Db.insert(newUser, newUserAccount); //the newUser would really just be the primary key in the db...
}
void AppUserDb::addCampaign(QString adCampaignOwnerName, QString adCampaignName)
{
    if(m_Db.contains(adCampaignOwnerName))
    {
        AppUserAccount *userAccount = m_Db.value(adCampaignOwnerName);
        SiteOwnerAdCampaign *newAdCampaign = new SiteOwnerAdCampaign();
        newAdCampaign->CampaignName = adCampaignName;
        userAccount->SiteOwnerAdCampaignsList.append(newAdCampaign);
        emit d("added ad campaign to ap db: " + adCampaignName + " with owner: " + adCampaignOwnerName);
    }
    else
    {
        emit d("error in addCampaign, user not found in db");
    }
}
