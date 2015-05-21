#ifndef ICLEANROOMSESSIONREQUEST_H
#define ICLEANROOMSESSIONREQUEST_H

class ICleanRoomSession;

class ICleanRoomSessionRequest
{
public:
    ICleanRoomSessionRequest(ICleanRoomSession *session);
    void respond(ICleanRoomSessionResponse *response);
private:
    ICleanRoomSession *m_Session;
};

#endif // ICLEANROOMSESSIONREQUEST_H
