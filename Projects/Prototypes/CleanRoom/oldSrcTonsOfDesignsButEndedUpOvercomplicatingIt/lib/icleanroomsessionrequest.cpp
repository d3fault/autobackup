#include "icleanroomsessionrequest.h"

#include "icleanroomsession.h"

ICleanRoomSessionRequest::ICleanRoomSessionRequest(ICleanRoomSession *session)
    : m_Session(session)
{ }
void ICleanRoomSessionRequest::respond(ICleanRoomSessionResponse *response)
{
    m_Session->respond(this, response);
}
