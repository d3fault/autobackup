#include "icleanroom.h"

#include "cleanroomsession.h"
#include "icleanroomnewsessionrequest.h"
#include "icleanroomfrontpagedefaultviewrequest.h"

ICleanRoom::ICleanRoom(QObject *parent)
    : QObject(parent)
{ }
void ICleanRoom::initializeAndStart()
{
    //wait for business to become ready etc, then:
    emit readyForSessions();
}
void ICleanRoom::newSession(ICleanRoomNewSessionRequest *request)
{
    request->respond(CleanRoomSession(this));
}
