%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#ifndef %API_NAME_UPPERCASE%GUIWIDGET_H
#define %API_NAME_UPPERCASE%GUIWIDGET_H

#include <QWidget>
#include <QScopedPointer>

#include "%API_NAME_LOWERCASE%session.h"

class I%API_NAME%;

class %API_NAME%GuiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit %API_NAME%GuiWidget(I%API_NAME% *%API_AS_VARIABLE_NAME%, QWidget *parent = 0);
private:
    QScopedPointer<%API_NAME%Session> m_Session;
public slots:
    void handleNewSessionCreated(%API_NAME%Session session);

    %API_FRONTEND_SKELETON_CALLBACK_METHOD_DECLARATIONS%
};

#endif // %API_NAME_UPPERCASE%GUIWIDGET_H
