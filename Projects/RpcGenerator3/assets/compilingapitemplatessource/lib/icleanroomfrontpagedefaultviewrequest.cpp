#include "icleanroomfrontpagedefaultviewrequest.h"

#include "icleanroom.h"

ICleanRoomFrontPageDefaultViewRequest::ICleanRoomFrontPageDefaultViewRequest(ICleanRoom *cleanRoom, double someArg0, QObject *parent)
    : QObject(parent)
    , m_SomeArg0(someArg0)
{
    connect(this, &ICleanRoomFrontPageDefaultViewRequest::frontPageDefaultViewRequested, cleanRoom, &ICleanRoom::getFrontPageDefaultView);
}
void ICleanRoomFrontPageDefaultViewRequest::processRequest()
{
    emit frontPageDefaultViewRequested(this, m_SomeArg0);
}
