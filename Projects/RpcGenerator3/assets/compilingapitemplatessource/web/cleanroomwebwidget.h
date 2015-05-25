#ifndef CLEANROOMWEBWIDGET_H
#define CLEANROOMWEBWIDGET_H

#include <QStringList>

class CleanRoomSession;

class CleanRoomWebWidget : public WApplication
{
public:
    CleanRoomWebWidget(const WEnvironment &myEnv);
    static WApplication *cleanRoomWebWidgetEntryPoint(const WEnvironment &myEnv);
private:
    friend class CleanRoomWeb;
    CleanRoomSession *m_Session;

    QList<WLabel*> m_CleanRoomDocsWidgets;

    void handleCleanRoomSessionStarted(CleanRoomSession *session);

    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMWEBWIDGET_H
