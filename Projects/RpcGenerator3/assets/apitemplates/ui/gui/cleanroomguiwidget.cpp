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
#if 0
void %API_NAME%GuiWidget::handleFrontPageDefaultViewReceived(QStringList frontPageDocs)
{
    int max = NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(frontPageDocs.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        m_%API_NAME%DocsWidgets.at(i)->setText(frontPageDocs.at(i));
    }
    setDisabled(false);
}
#endif
%API_QTFRONTEND_SKELETON_CALLBACK_METHOD_DEFINITIONS%
