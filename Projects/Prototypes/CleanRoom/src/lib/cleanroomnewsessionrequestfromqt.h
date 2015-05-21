#ifndef CLEANROOMNEWSESSIONREQUESTFROMQT_H
#define CLEANROOMNEWSESSIONREQUESTFROMQT_H

#include "icleanroomnewsessionrequest.h"

class CleanRoomNewSessionRequestFromQt : public ICleanRoomNewSessionRequest
{
public:
    CleanRoomNewSessionRequestFromQt(CleanRoom *cleanRoom, QObject *objectToRespondTo, const char *slotCallback)
        : ICleanRoomNewSessionRequest(cleanRoom)
        , m_ObjectToRespondTo(objectToRespondTo)
        , m_SlotCallback(slotCallback)
    { }
    //void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
    void respond(CleanRoomSession *session);
private:
    QObject *m_ObjectToRespondTo;
    std::string m_SlotCallback;
};

#endif // CLEANROOMNEWSESSIONREQUESTFROMQT_H
