#include "adcaptchasite.h"

AdCaptchaSite::AdCaptchaSite(WContainerWidget *parent)
    : WContainerWidget(parent),
      m_CampaignEditorView(0),
      m_AdEditorView(0)
      //initialize every view on the site to 0. when the internal path changes, instantiate them (if not 0) and then make them the current top. if they're already instantiated, just make them the current top. we re-use the instantiations, only instantiating each view a) when it's viewed and b) once per user, no matter if they navigate to each one hundreds of times
{
    m_MyDb.getLogin().changed().connect(this, &AdCaptchaSite::handleLoginChanged);

    Wt::Auth::AuthWidget *authWidget = new Wt::Auth::AuthWidget(Database::getAuthService(), session_.users(), m_MyDb.getLogin());
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
