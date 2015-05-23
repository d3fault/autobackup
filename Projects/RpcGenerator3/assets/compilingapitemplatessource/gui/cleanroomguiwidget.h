#ifndef CLEANROOMGUIWIDGET_H
#define CLEANROOMGUIWIDGET_H

#include <QWidget>

class CleanRoomSession;

class CleanRoomGuiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CleanRoomGuiWidget(QWidget *parent = 0);
private:
    CleanRoomSession *m_Session;
public slots:
    void handleNewSessionCreated(CleanRoomSession *session);
    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMGUIWIDGET_H
