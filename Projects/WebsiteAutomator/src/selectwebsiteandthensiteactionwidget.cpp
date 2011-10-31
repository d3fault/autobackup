#include "selectwebsiteandthensiteactionwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QList>
#include <websiteautomator.h>

//TODOreq: this widget needs to have 2 lists on it: website and website action. you select a website on the list on the left, the one on the right is populated with the list of SiteAction's for that website. you select the site action and then press "Go".  That opens up a new "GoWidget", which runs on it's own background thread. you can have unlimited "GoWidgets" open at a time. you just close it to stop it (or press "stop" lol). for now, assume "4chan" and "post thread" for the lists, only doing a Go button that launches a GoWidget

selectWebsiteAndThenSiteActionWidget::selectWebsiteAndThenSiteActionWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_GoButton = new QPushButton(tr("Start")); //TODOopt: translations... tr()
    m_Layout->addWidget(m_GoButton);
    this->setLayout(m_Layout);

    connect(m_GoButton, SIGNAL(clicked()), this, SLOT(handleGoButtonClicked()));


    //TODOreq: debug code below. we need to populate the lists based on data from the server.
    Step *submitStep = new Step("some parcellizeable way of signifying that we want to fill out the name, email address, body, and image field (the captcha is implicit/automatic)");
    //i could put each input "name" and "email address" etc as it's own step, but then it might fuck up the automated captcha stuff because i'd be checking for it's existence after each and every step (well, before and after each, i think)
    postThreadAction.addStep(submitStep);
    fourChanWebsite.Actions.append(postThreadAction);
    Websites.append(fourChanWebsite);
    //TODOreq: fourchanWebsite needs to be parcelable, which parcelizes all the actions and all their steps...
    //technically, it need only be read-only parcelable... we populate the object from a parcelized version pulled from the site..
    //..but to future proof it (when i outsource the creation of definitions), i should make it both readwrite parcelable

    //TODO: idk where this would go, but there needs to be like "variable chaining". one of the variables when posting to craigslist is a random string used to identify an input field. you must first parse out the input field name (random string), then you must also attach a piece of input to that input field name for submission.. a 2 step process. complicates shit even more. my brain is seriously going to explode considering all these possibilities

    //as much as i want to write a simplified case... say, posting to 4chan... i want to make sure the design is scalable as fuck to any/all scenarios...
}
selectWebsiteAndThenSiteActionWidget::~selectWebsiteAndThenSiteActionWidget()
{

}
void selectWebsiteAndThenSiteActionWidget::handleGoButtonClicked()
{
    WebsiteAutomator *newWebsiteAutomator(new WebsiteAutomator(Websites["4chan"].Actions["PostThread"]));
    m_WebsiteAutomatorList->append(newWebsiteAutomator);
    newWebsiteAutomator->startAutomating();
}
