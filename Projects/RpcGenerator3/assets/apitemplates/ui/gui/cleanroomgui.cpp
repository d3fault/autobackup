%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include "%API_NAME_LOWERCASE%gui.h"

#include "%API_NAME_LOWERCASE%.h"

%API_NAME%Gui::%API_NAME%Gui(QObject *parent)
    : QObject(parent)
    , m_%API_NAME%(new %API_NAME%(this))
{
    connect(m_%API_NAME%, &I%API_NAME%::readyForSessions, this, &%API_NAME%Gui::handle%API_NAME%ReadyForSessions);
    connect(this, &%API_NAME%Gui::initializeAndStart%API_NAME%Requested, m_%API_NAME%, &I%API_NAME%::initializeAndStart);
    emit initializeAndStart%API_NAME%Requested();
}
void %API_NAME%Gui::handle%API_NAME%ReadyForSessions()
{
    m_Gui.reset(new %API_NAME%GuiWidget(m_%API_NAME%));
    m_Gui->show();
}
