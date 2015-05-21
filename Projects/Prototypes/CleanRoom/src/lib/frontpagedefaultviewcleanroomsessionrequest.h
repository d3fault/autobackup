#ifndef FRONTPAGEDEFAULTVIEWCLEANROOMSESSIONREQUEST_H
#define FRONTPAGEDEFAULTVIEWCLEANROOMSESSIONREQUEST_H

#include "icleanroomsessionrequest.h"

class FrontPageDefaultViewCleanRoomSessionRequest : public ICleanRoomSessionRequest
{
public:
    FrontPageDefaultViewCleanRoomSessionRequest(ICleanRoomSession *session);
    void processRequest();
};

#endif // FRONTPAGEDEFAULTVIEWCLEANROOMSESSIONREQUEST_H
