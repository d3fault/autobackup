%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include <Wt/WEnvironment>
using namespace Wt;
#include "%API_NAME_LOWERCASE%webwidget.h"

#include <boost/bind.hpp>

I%API_NAME% *%API_NAME%WebWidget::s_%API_NAME% = NULL;

%API_NAME%WebWidget::%API_NAME%WebWidget(const WEnvironment &environment, WtLibVersion version)
    : WApplication(environment, version)
{
    if(environment.ajax())
        this->enableUpdates(true);
    else
        this->deferRendering();

    %API_NAME%Session::requestNewSession(s_%API_NAME%, this->sessionId(), boost::bind(&%API_NAME%WebWidget::handle%API_NAME%SessionStarted, this, _1));
}
WApplication *%API_NAME%WebWidget::%API_AS_VARIABLE_NAME%WebWidgetEntryPoint(const WEnvironment &myEnv)
{
    return new %API_NAME%WebWidget(myEnv);
}
void %API_NAME%WebWidget::handle%API_NAME%SessionStarted(%API_NAME%Session session)
{
    if(!environment().ajax())
        this->resumeRendering();
    m_Session.reset(new %API_NAME%Session(session));
}
%API_WTFRONTEND_SKELETON_CALLBACK_METHOD_DEFINITIONS%
