%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include "%API_NAME_LOWERCASE%guiwidget.h"

%API_NAME%GuiWidget::%API_NAME%GuiWidget(I%API_NAME% *%API_AS_VARIABLE_NAME%, QWidget *parent)
    : QWidget(parent)
{
    setDisabled(true);
    %API_NAME%Session::requestNewSession(%API_AS_VARIABLE_NAME%, this, SLOT(handleNewSessionCreated(%API_NAME%Session)));
}
void %API_NAME%GuiWidget::handleNewSessionCreated(%API_NAME%Session session)
{
    setDisabled(false);
    m_Session.reset(new %API_NAME%Session(session));
}
%API_QTFRONTEND_SKELETON_CALLBACK_METHOD_DEFINITIONS%
