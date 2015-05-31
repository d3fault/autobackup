#include "icleanroomfrontpagedefaultviewrequest.h"

#include "icleanroom.h"

ICleanRoomFrontPageDefaultViewRequest::ICleanRoomFrontPageDefaultViewRequest(ICleanRoom *cleanRoom, CleanRoomSession parentSession, double someArg0, QObject *parent)
    : QObject(parent)
    , m_ParentSession(parentSession) //The reason this isn't a part of ICleanRoomRequest, is because there's one kind of request that doesn't have a parent session: a NewSessionRequest!
    , m_SomeArg0(someArg0)
{
    connect(this, &ICleanRoomFrontPageDefaultViewRequest::frontPageDefaultViewRequested, cleanRoom, &ICleanRoom::getFrontPageDefaultView);
}
CleanRoomSession ICleanRoomFrontPageDefaultViewRequest::parentSession() const
{
    return m_ParentSession;
}
void ICleanRoomFrontPageDefaultViewRequest::processRequest()
{
    emit frontPageDefaultViewRequested(this, m_SomeArg0);
}
