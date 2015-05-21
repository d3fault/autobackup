#ifndef CLEANROOMSESSION
#define CLEANROOMSESSION

#include <QStringList>

#include <boost/function.hpp>

#include "icleanroomrequest.h"

class QObject;

class CleanRoom;

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
    static void requestNewSession(CleanRoom *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession*)> wApplicationCallback);
    void requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot);
    void requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback);
private:
    CleanRoom *m_CleanRoom;
    static void invokeRequest(ICleanRoomRequest *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // CLEANROOMSESSION

