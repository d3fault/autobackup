#ifndef ICLEANROOMNEWSESSIONREQUEST
#define ICLEANROOMNEWSESSIONREQUEST

#include "cleanroom.h"
#include "icleanroomrequest.h"

class ICleanRoomNewSessionRequest : public ICleanRoomRequest
{
public:
    ICleanRoomNewSessionRequest(CleanRoom *cleanRoom)
        : ICleanRoomRequest(cleanRoom)
    { }
    void invokeSlotThatHandlesRequest()
    {
        QMetaObject::invokeMethod(m_CleanRoom, "newSession", Q_ARG(ICleanRoomRequest*, this));
    }
};

#endif // ICLEANROOMNEWSESSIONREQUEST
