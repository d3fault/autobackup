#ifndef CLEANROOMGUIWIDGET_H
#define CLEANROOMGUIWIDGET_H

#include <QWidget>

class CleanRoomSession;

class CleanRoomGuiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CleanRoomGuiWidget(QWidget *parent = 0);
    ~CleanRoomGuiWidget();
private:
    CleanRoomSession *m_Session;
private slots:
    void handleCleanRoomSessionStarted(CleanRoomSession *session);
    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMGUIWIDGET_H
