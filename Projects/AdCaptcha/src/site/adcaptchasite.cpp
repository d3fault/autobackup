#include "adcaptchasite.h"

AdCaptchaSite::AdCaptchaSite(WContainerWidget *parent)
    : WContainerWidget(parent),
      m_CampaignEditorView(0),
      m_AdEditorView(0),
      m_HomeView(0)
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
        //TODO: since this seems to be the first point of entry after the user first registers... we can check our app bank cache db to see if we have set up a (non-cached) bank account yet on our remote server. if we haven't, now is where we do it and we store the user/id (whatever) in the app bank cache db too. we init it to zero on user creation, then check to see if it's still zero right here. if it is, we know they haven't set up a bank account yet. we want to delay this until after the registration CONFIRMATION process, so spam creating accounts (without verifying them) doesn't even send a message to our remote bank server. ya know, i kinda like that: RemoteBankServer and LocalAppBankCache. they are the same except remote manages multiple app bank accounts (for future projects)
        handleInternalPathChanged(WApplication::instance()->internalPath());
    }
    else
    {
        m_ViewStack->clear();
        m_HomeView = 0;
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
        m_HomeView = new AdCaptchaSiteHome(m_ViewStack);
    }
    m_ViewStack->setCurrentWidget(m_HomeView);
}
void AdCaptchaSite::showOwner()
{
    if(!m_CampaignEditorView)
    {
        m_CampaignEditorView = new OwnerCampaignsView(&m_MyDb, m_ViewStack);
    }
    m_ViewStack->setCurrentWidget(m_CampaignEditorView);
}
void AdCaptchaSite::showPublisher()
{
    if(!m_AdEditorView)
    {
        m_AdEditorView = new PublisherCampaignsView(&m_MyDb, m_ViewStack);
    }
    m_ViewStack->setCurrentWidget(m_AdEditorView);
}
std::string AdCaptchaSite::getHomePath()
{
    return HOME_PATH;
}
std::string AdCaptchaSite::getOwnerPath()
{
    return OWNER_PATH;
}
std::string AdCaptchaSite::getPublisherPath()
{
    return PUBLISHER_PATH;
}
