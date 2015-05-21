#include "frontpagedefaultviewcleanroomsessionrequest.h"

#include "cleanroom.h"

FrontPageDefaultViewCleanRoomSessionRequest::FrontPageDefaultViewCleanRoomSessionRequest(CleanRoomSession *session)
    : ICleanRoomSessionRequest(session)
{ }
void FrontPageDefaultViewCleanRoomSessionRequest::processRequest()
{
    //cleanRoom()->getFrontPageDefaultViewBegin(this);
}
