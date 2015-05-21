#include "icleanroomsessionrequest.h"

#include "cleanroomsession.h"

CleanRoom *ICleanRoomSessionRequest::cleanRoom() const
{
    return m_Session->m_CleanRoom;
}
