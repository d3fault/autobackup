%RPC_AUTO_SKELETON_FILE_SUGGESTIONANDWARNING%
#ifndef %API_NAME_UPPERCASE%GUI_H
#define %API_NAME_UPPERCASE%GUI_H

#include <QObject>
#include <QScopedPointer>

#include "%API_NAME_LOWERCASE%guiwidget.h"

class I%API_NAME%;

class %API_NAME%Gui : public QObject
{
    Q_OBJECT
public:
    explicit %API_NAME%Gui(QObject *parent = 0);
private:
    I%API_NAME% *m_%API_NAME%;
    QScopedPointer<%API_NAME%GuiWidget> m_Gui;
signals:
    void initializeAndStart%API_NAME%Requested();
private slots:
    void handle%API_NAME%ReadyForSessions();
};

#endif // %API_NAME_UPPERCASE%GUI_H
