#include "publishercampaignsview.h"

PublisherCampaignsView::PublisherCampaignsView(Database *dbInstance, WContainerWidget *parent)
    : WContainerWidget(parent),
      m_MyDb(dbInstance)
{
    //TODO: get list of campaigns, and do "add new campaign" button. i still need to work out the logistics of how the ads will be paid for etc. doubles? regular mode actually sounds harder..
    //how do most do it? credits are gradually deducted and then their ad is no longer shown until they add more credits? an email sent out when their credits get low. can they set a max pay-per-view? what about pay-per-click? doubles mode sounds easier... and is also easier on the bandwidth as it can be cached on the site owner's site. very tempted to just say FUCK IT and do doubles mode. their can be an auction like view where you pre-order days of ads for a certain site in advance. easier to code, still "maximizes profits" (somewhat).
    //i'm undecided.. but learning towards doubles.
    //it might be wiser to rename the whole thing "ad doubles" or whatever and have both non-captcha ads and adcaptcha ads
    //it sounds eaiser.
    //the auction view can have a filter to show captcha or non-captcha types (shows both by default).
    //i think both is optimal. AdDoubles or whatever being the main (domain), adcaptcha.com pointing to addoubles.com/captcha
    //EXECUTIVE DECISION: FUCK REGULAR MODE. pay-per-view and pay-per-click and FUCK ALL THAT SHIT MAKES IT SO COMPLICATED
    //meh i need to design this shit before i progress.
    addWidget(new WText("Ad Publisher Campaigns View"));
    addWidget(new WBreak());
    addWidget(new WText("xD LoL"));
    addWidget(new WBreak());
    WAnchor *ownerAnchor = new WAnchor(AdCaptchaSite::getOwnerPath(), "Owner Page");
    ownerAnchor->setRefInternalPath(AdCaptchaSite::getOwnerPath());
    addWidget(ownerAnchor);
}
