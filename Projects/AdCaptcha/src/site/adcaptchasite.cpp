#include "adcaptchasite.h"

AdCaptchaSite::AdCaptchaSite(WContainerWidget *parent)
    : WContainerWidget(parent),
      m_CampaignEditorView(0),
      m_AdEditorView(0)
      //initialize every view on the site to 0. when the internal path changes, instantiate them (if not 0) and then make them the current top. if they're already instantiated, just make them the current top. we re-use the instantiations, only instantiating each view a) when it's viewed and b) once per user, no matter if they navigate to each one hundreds of times
{
    m_MyDb.getLogin().changed().connect(this, &AdCaptchaSite::handleLoginChanged);

    Wt::Auth::AuthWidget *authWidget = new Wt::Auth::AuthWidget(Database::getAuthService(), m_MyDb.getUserDatabase(), m_MyDb.getLogin());
    authWidget->addPasswordAuth(&Database::getPasswordService());
    authWidget->setRegistrationEnabled(true);

    this->addWidget(authWidget);

    m_ViewStack = new WStackedWidget();
    this->addWidget(m_ViewStack);

    WApplication::instance()->internalPathChanged().connect(this, &AdCaptchaSite::handleInternalPathChanged);

    authWidget->processEnvironment();
}
void AdCaptchaSite::handleLoginChanged()
{
    if(m_MyDb.getLogin().loggedIn())
    {
        handleInternalPathChanged(WApplication::instance()->internalPath());
    }
    else
    {
        m_ViewStack->clear();
        m_CampaignEditorView = 0;
        m_AdEditorView = 0;
    }
}
void AdCaptchaSite::handleInternalPathChanged(const std::string &newInternalPath)
{
    if(m_MyDb.getLogin().loggedIn())
    {
        if(newInternalPath == HOME_PATH)
        {
            showHome();
        }
        else if(newInternalPath == OWNER_PATH)
        {
            showOwner();
        }
        else if(newInternalPath == PUBLISHER_PATH) //could also catch cases like AdvertiserPath (just pure semantics, fuck it)
        {
            showPublisher();
        }
        else
        {
            WApplication::instance()->setInternalPath(HOME_PATH, true);
        }
    }
}
void AdCaptchaSite::showHome()
{
    if(!m_HomeView)
    {
        m_HomeView = new WContainerWidget(m_ViewStack);

        m_HomeView->addWidget(new WAnchor(OWNER_PATH, "For Website Owners - Shows Ads + Verify Humans"));
        m_HomeView->addWidget(new WBreak());
        m_HomeView->addWidget(new WAnchor(PUBLISHER_PATH, "For Advertisers/Publishers - Purchase Ad Space/Edit Current Ads"));
    }
    m_ViewStack->setCurrentWidget(m_HomeView);
}
void AdCaptchaSite::showOwner()
{
    if(!m_CampaignEditorView)
    {
        m_CampaignEditorView = new CampaignEditorView(&m_MyDb, m_ViewStack);
    }
    m_ViewStack->setCurrentWidget(m_CampaignEditorView);
}
void AdCaptchaSite::showPublisher()
{
    if(!m_AdEditorView)
    {
        m_AdEditorView = new AdEditorView(&m_MyDb, m_ViewStack);
    }
    m_ViewStack->setCurrentWidget(m_AdEditorView);
}
