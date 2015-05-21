#ifndef ICLEANROOMNEWSESSIONREQUEST
#define ICLEANROOMNEWSESSIONREQUEST

#include "cleanroom.h"
#include "icleanroomrequest.h"

class CleanRoomSession;

class ICleanRoomNewSessionRequest : public ICleanRoomRequest
{
public:
    ICleanRoomNewSessionRequest(CleanRoom *cleanRoom)
        : ICleanRoomRequest(cleanRoom)
    { }
    void invokeSlotThatHandlesRequest()
    {
        QMetaObject::invokeMethod(m_CleanRoom, "newSession", Q_ARG(ICleanRoomNewSessionRequest*, this));
    }
    virtual void respond(CleanRoomSession *newSession)=0;
#if 0
    {
        QMetaObject::invokeMethod(m_)
    }
#endif
};

#endif // ICLEANROOMNEWSESSIONREQUEST
