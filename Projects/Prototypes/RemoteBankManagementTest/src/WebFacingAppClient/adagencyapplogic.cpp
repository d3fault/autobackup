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
    connect(m_Db, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void AdAgencyAppLogic::purchaseSlotXforUserY(QString adCampaignNameToPurchaseSlotFrom, QString slotPurchaserUsername)
{
    //TODOreq: TODOTODOTODO -- after we do whatever needs to be done in this slot (initial check of allotted funds required for balance transfer), our bank server should listen to a signal from this level/layer (not the gui level/layer as i currently have it designed) and initiate the balance transfer.
    //it goes like this: Wt/GUI -> Wt/Front-End-Server (here) -> Qt/Back-End-Bank-Server
    //TODO: it makes no sense for us to emit a signal to the gui which then emits a signal to a server. we can hackily connect the two in the GUI code, however... keeping the interaction in the GUI code to a minimum. in the implementation the connect() would/should be done in THIS constructor's logic. the reason i didn't design it like that to begin with is because i didn't even have this class while i was designing it originally.

    //check in cache if enough funds availalbe
    //TODO

    //TODOsecure: check user's public cert for permission slip is valid, then put it into pending so no other users can try to buy it while we wait for our bank server to respond (should be fast, but never fast enough)
    //...

    //since we currently don't own the server in this class (which we SHOULD), we'll just emit the hacky signal that the GUI connects us to the server with and the server's code will do the verification. i mean, it still should... but it should do it before it's on the way towards sending the message to the back-end-server. a boolean m_Server.hasEnoughFundsForBalanceTransfer(username) or something... and we call it blocking queued i guess?? that m_Server is really the LocalAppBankCache class, and it only checks locally at first. (then sets the campaign as pendingBuy, then the server has to also confirm the amount and also do the balance transfer. then when we get back here, we update our cache to reflect the balance transfer)

    emit appLogicRequestsSlotPurchase(slotPurchaserUsername, adCampaignNameToPurchaseSlotFrom.Owner, adCampaignNameToPurchaseSlotFrom.currentPrice);
    //we're going to need to remember what slot wants to be purchased.. because the bank server has no need to know. only the owner's username and the purchaser's username and the price
    //i'm really starting to think i should just start coding the Wt/functioning app...
    //but i also don't want to for 2 reasons: a) want to test persistence of remote bank db, and b) want to prototype OptionallySecureAsFuckBank thingo....

    //if i was better at git i'd revert it to before this applogic code into a new folder (while still keeping this app logic code) where i could prototype the db persistency. after that i'd be confident enough to start the Wt/functional attempt (because it might take multiple attempts). i think i could add the SecureAsFuck functionality/features later on (famous last words??).. they are just custom alternate paths that aren't HUGE (but do HUGE THINGS) from the big picture. it would be hard'ish to prototype (as is this right here) without an actual/functioning app...
}
