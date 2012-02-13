#include "adcaptchasitehome.h"

AdCaptchaSiteHome::AdCaptchaSiteHome(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    //addWidget(new WAnchor(AdCaptchaSite::getOwnerPath(), "For Website Owners - Shows Ads + Verify Humans"));
    //addWidget(new WBreak());
    //addWidget(new WAnchor(AdCaptchaSite::getPublisherPath(), "For Advertisers/Publishers - Purchase Ad Space/Edit Current Ads"));
    //addWidget(new WText("hallo"));

    WVBoxLayout *layout = new WVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    layout->addWidget(new WText("okay..."), 0, AlignCenter | AlignMiddle);

    WAnchor *pubAnchor = new WAnchor(AdCaptchaSite::getPublisherPath(), "Publishers");
    pubAnchor->setRefInternalPath(AdCaptchaSite::getPublisherPath());
    layout->addWidget(pubAnchor, 0, AlignCenter | AlignMiddle);

    WAnchor *ownAnchor = new WAnchor(AdCaptchaSite::getOwnerPath(), "Owners");
    ownAnchor->setRefInternalPath(AdCaptchaSite::getOwnerPath());
    layout->addWidget(ownAnchor, 0, AlignCenter | AlignMiddle);

    this->setLayout(layout);
}
