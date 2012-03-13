#include "adagencyapplogic.h"

//this is just to encapsulate the application logic that is only present on the front-end/cache/appId-specific-app. it doesn't have to be an ad agency... it just happens to be what i want to make first
//i sort of want to do a "register user" (change functionality of existing "Add User" button), which just registers a user in the ad agency.. then below it do a confirm user.. then below it do a log in user (which is where i check if it's the first login.. which then triggers my existing "AddUser" (bank) logic... as is how it will function in the implementation... but i also kind of consider that a waste of time. i should just focus on the slot purchasing/balance transfer logic. that's what i need to figure out... that's what's more confusing in mah head
AdAgencyAppLogic::AdAgencyAppLogic(QObject *parent) :
    QObject(parent)
{
}
void AdAgencyAppLogic::addUser(QString newUser)
{
    m_Db->addUser(newUser);
}
void AdAgencyAppLogic::createAdCampaignXForUserY(QString adCampaignName, QString adCampaignOwnerName)
{
    m_Db->addCampaign(adCampaignOwnerName, adCampaignName);
    emit campaignAdded(adCampaignName); //this would really be an object that also has a pointer to it's owner. we'll probably have to create some hacky logic to keep track of owners by campaignName
}
void AdAgencyAppLogic::init()
{
    m_Db = new AppUserDb(); //i should probably move the db for the local bank cache into it's init as well
}
