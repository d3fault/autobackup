#ifndef CLEANROOMNEWSESSIONREQUESTFROMQT_H
#define CLEANROOMNEWSESSIONREQUESTFROMQT_H

#include "icleanroomnewsessionrequest.h"

class CleanRoomSession;

class CleanRoomNewSessionRequestFromQt : public ICleanRoomNewSessionRequest
{
    Q_OBJECT
public:
    CleanRoomNewSessionRequestFromQt(ICleanRoom *cleanRoom, QObject *objectToRespondTo, const char *slotCallback, QObject *parent = 0)
        : ICleanRoomNewSessionRequest(cleanRoom, parent)
        , m_ObjectToRespondTo(objectToRespondTo)
    {
        QObject::connect(this, SIGNAL(newSessionResponseRequested(CleanRoomSession*)), objectToRespondTo, slotCallback);
    }
    void respond(CleanRoomSession *session);
private:
    QObject *m_ObjectToRespondTo;
signals:
    void newSessionResponseRequested(CleanRoomSession *session);
};

#endif // CLEANROOMNEWSESSIONREQUESTFROMQT_H
