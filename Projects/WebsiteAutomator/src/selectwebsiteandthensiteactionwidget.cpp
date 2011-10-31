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
}
selectWebsiteAndThenSiteActionWidget::~selectWebsiteAndThenSiteActionWidget()
{

}
void selectWebsiteAndThenSiteActionWidget::handleGoButtonClicked()
{
    m_WebsiteAutomatorList->append(new WebsiteAutomator(Websites["4chan"].Actions["PostThread"].getDefinition()));
}
