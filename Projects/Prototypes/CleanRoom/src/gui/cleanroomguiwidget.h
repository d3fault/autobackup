#ifndef CLEANROOMGUIWIDGET_H
#define CLEANROOMGUIWIDGET_H

#include <QWidget>

#include <QVariantList>

class CleanRoomSession;

class CleanRoomGuiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CleanRoomGuiWidget(QWidget *parent = 0);
private:
    CleanRoomSession *m_Session;
private slots:
    void handleNewSessionCreated(CleanRoomSession *session);
    void handleFrontPageDefaultViewReceived(QVariantList frontPageDocsVariantList);
};

#endif // CLEANROOMGUIWIDGET_H
