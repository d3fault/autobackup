#include "anonymousbitcoincomputing.h"

AnonymousBitcoinComputing::AnonymousBitcoinComputing(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    //i want a WMenu that is site-wide that controls the contents of the m_ViewStack.

    //[Index], [Balance], [Advertising]
    //Index is just that, an index of all sites that a) advertise and b) contain advertisements. contrast with google's 'search engine' index...
    //Balance has sub-types "Add Funds", "Request Payout", "Payout Options"... and maybe a "Previous Funds Added Transactions" where they can refresh a previously used 'add funds key' in case they were stupid and paid to it again after a previous completion/confirmation
    //Advertising has sub-types "Advertise Your Page! - Get Known!" and "Sell Owned Ad Space! - Make Money!" or something. AdCaptcha would fall under the Advertising Category (but would also maybe have another reference to it under some other menu too ("Human Verification" maybe?)
    //TODO: other types include "Servers": "Sell Bandwidth/Space" and "Buy Bandwidth/Space". err, the bandwidth is purchased, but the space is rented. all just words anyways..
}
void AnonymousBitcoinComputing::zeroOutEachViewPointer()
{
    //assign each of the pointers to views that are in the view stack to zero. there are a few times we zero them out, so putting this in a helper function just reduces lines. on init, we zero em out... because when instantiate them we check to see if they are equal to zero... and on user logout, for example.. we zero them out again

    //m_AdSlotPurchaseView = 0; //the page where you can purchase slots. a huge filterable/sortable list
    //m_ListOwnedSitesView = 0; //the page where you can list your own sites, which then show up for others on the m_AdSlotPurchaseView. it's just a simple list of sites, with an "add new" field/button at the bottom. you need to prove ownership of course (meta tags, file in root folder, etc)
    //m_AddFundsView = 0; //precisely what it says. gives you a new key, shows pending balance, ajax updates to confirmed when appropriate
}
