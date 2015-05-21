#ifndef CLEANROOMSESSION
#define CLEANROOMSESSION

#include <boost/function.hpp>

#include "icleanroomrequest.h"

class QObject;

class CleanRoom;

class CleanRoomFrontPageDefaultViewRequestFromQt;
class CleanRoomFrontPageDefaultViewRequestFromWt;


class CleanRoomSession
{
public:
    CleanRoomSession(CleanRoom *cleanRoom)
        : m_CleanRoom(cleanRoom)
    { }
    CleanRoom *cleanRoom() const
    {
        return m_CleanRoom;
    }
    void requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot);
    void requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QVariantList)> wApplicationCallback);
private:
    CleanRoom *m_CleanRoom;
    static void invokeRequest(ICleanRoomRequest *requestToInvoke)
    {
        requestToInvoke->invokeSlotThatHandlesRequest();
    }
};

#endif // CLEANROOMSESSION

