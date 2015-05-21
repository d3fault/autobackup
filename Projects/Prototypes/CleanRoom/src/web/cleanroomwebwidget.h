#ifndef CLEANROOMWEBWIDGET_H
#define CLEANROOMWEBWIDGET_H

#include <QVariant>

class WApplication
{ };

class CleanRoomSession;

class CleanRoomWebWidget : public WApplication
{
private:
    CleanRoomSession *m_Session;

    void handleCleanRoomSessionStarted(CleanRoomSession *session);
    void handleFrontPageDefaultViewReceived(QVariantList frontPageDocsVariantList);
};

#endif // CLEANROOMWEBWIDGET_H
