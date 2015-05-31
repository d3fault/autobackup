%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include <Wt/WServer>
using namespace Wt;

#include "%API_NAME_LOWERCASE%web.h"

#include <boost/bind.hpp>

#include "%API_NAME_LOWERCASE%.h"
#include "%API_NAME_LOWERCASE%session.h"

#include "%API_NAME_LOWERCASE%webwidget.h"

%API_NAME%Web::%API_NAME%Web(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_%API_NAME%(new %API_NAME%(this))
{
    m_WtServer.reset(new WServer(argv[0]));
    m_WtServer->setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    m_WtServer->addEntryPoint(Application, &%API_NAME%WebWidget::%API_AS_VARIABLE_NAME%WebWidgetEntryPoint);

    connect(m_%API_NAME%, &%API_NAME%::readyForSessions, this, &%API_NAME%Web::handle%API_NAME%ReadyForSessions);
    connect(this, &%API_NAME%Web::initializeAndStartRequested, m_%API_NAME%, &%API_NAME%::initializeAndStart);
    emit initializeAndStartRequested();
}
%API_NAME%Web::~%API_NAME%Web()
{
    if((!m_WtServer.isNull()) && m_WtServer->isRunning())
        m_WtServer->stop();
}
void %API_NAME%Web::handle%API_NAME%ReadyForSessions()
{
    if(m_WtServer.isNull() || m_WtServer->isRunning())
        return;
    %API_NAME%WebWidget::s_%API_NAME% = m_%API_NAME%;
    m_WtServer->start();
}
