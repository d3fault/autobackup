#include "frontpagedefaultviewcleanroomsessionrequest.h"

#include "cleanroom.h"

FrontPageDefaultViewCleanRoomSessionRequest::FrontPageDefaultViewCleanRoomSessionRequest(ICleanRoomSession *session)
    : ICleanRoomSessionRequest(session)
{ }
void FrontPageDefaultViewCleanRoomSessionRequest::processRequest()
{
    cleanRoom()->getFrontPageDefaultViewBegin(this);
}
