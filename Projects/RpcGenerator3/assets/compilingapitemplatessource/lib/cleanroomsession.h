#ifndef CLEANROOMSESSION
#define CLEANROOMSESSION

#include <QVariant> //TODOreq: proper includes for the request/response arg types. but actually I _THINK_ QVariant does provide compile time type checking so maybe this include QVariant is actually good-enough/elegant

#include <boost/function.hpp>

#include "icleanroomrequest.h"

class QObject;

class ICleanRoom;

class CleanRoomSession
{
public:
    CleanRoomSession(ICleanRoom *cleanRoom)
        : m_CleanRoom(cleanRoom)
    { }
    static void requestNewSession(ICleanRoom *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(ICleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession*)> wApplicationCallback);
    void requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot, double someArg0);
    void requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback, double someArg0);
private:
    ICleanRoom *m_CleanRoom;
    static void invokeRequest(ICleanRoomRequest *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // CLEANROOMSESSION

