%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#include <Wt/WApplication>
using namespace Wt;
#ifndef %API_NAME_UPPERCASE%WEBWIDGET_H
#define %API_NAME_UPPERCASE%WEBWIDGET_H

#include <QScopedPointer>

#include "%API_NAME_LOWERCASE%session.h"

class I%API_NAME%;

class %API_NAME%WebWidget : public WApplication
{
public:
    static I%API_NAME% *s_%API_NAME%;

    %API_NAME%WebWidget(const WEnvironment &myEnv, WtLibVersion version = WT_INCLUDED_VERSION);
    static WApplication *%API_AS_VARIABLE_NAME%WebWidgetEntryPoint(const WEnvironment &myEnv);
private:
    friend class %API_NAME%Web;
    QScopedPointer<%API_NAME%Session> m_Session;

    void handle%API_NAME%SessionStarted(%API_NAME%Session session);

    %API_FRONTEND_SKELETON_CALLBACK_METHOD_DECLARATIONS%
};

#endif // %API_NAME_UPPERCASE%WEBWIDGET_H
