#ifndef CLEANROOMWEBWIDGET_H
#define CLEANROOMWEBWIDGET_H

#include <QStringList>

class WApplication
{ };

class CleanRoomSession;

class CleanRoomWebWidget : public WApplication
{
private:
    friend class CleanRoomWeb;
    CleanRoomSession *m_Session;

    void handleCleanRoomSessionStarted(CleanRoomSession *session);

    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMWEBWIDGET_H
