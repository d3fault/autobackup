#ifndef CLEANROOMSESSION
#define CLEANROOMSESSION

#include <QSharedData>

#include <boost/function.hpp>

#include "icleanroomrequest.h"

class QObject;

class ICleanRoom;

class CleanRoomSessionData : public QSharedData
{
public:
    CleanRoomSessionData(ICleanRoom *cleanRoom)
        : m_CleanRoom(cleanRoom)
        , m_LoggedIn(false)
    { }
    CleanRoomSessionData(const CleanRoomSessionData &other)
        : QSharedData(other)
        , m_CleanRoom(other.m_CleanRoom)
        , m_LoggedIn(other.m_LoggedIn)
    { }
    ~CleanRoomSessionData()
    { }
    ICleanRoom *m_CleanRoom;
    bool m_LoggedIn;
};

class CleanRoomSession
{
public:
    CleanRoomSession(ICleanRoom *cleanRoom)
    {
        d = new CleanRoomSessionData(cleanRoom);
    }
    CleanRoomSession(const CleanRoomSession &other)
        : d(other.d)
    { }

    bool loggedIn() const { return d->m_LoggedIn; }
    void setLoggedIn(bool loggedIn) { d->m_LoggedIn = loggedIn; }

    ICleanRoom *cleanRoom() const { return d->m_CleanRoom; }

    static void requestNewSession(ICleanRoom *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(ICleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession*)> wApplicationCallback);

    void requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot, double someArg0) const;
    void requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback, double someArg0) const;
private:
    QSharedDataPointer<CleanRoomSessionData> d;

    static void invokeRequest(ICleanRoomRequest *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // CLEANROOMSESSION
