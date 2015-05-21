#ifndef ICLEANROOMSESSIONREQUEST_H
#define ICLEANROOMSESSIONREQUEST_H

class CleanRoom;
class ICleanRoomSession;

class ICleanRoomSessionRequest
{
public:
    ICleanRoomSessionRequest(ICleanRoomSession *session)
        : m_Session(session)
    { }
    virtual void processRequest()=0;
private:
    friend class ICleanRoomSessionResponder;
    ICleanRoomSession *m_Session;

    CleanRoom *cleanRoom() const
    {
        return m_Session->m_CleanRoom;
    }
};

#endif // ICLEANROOMSESSIONREQUEST_H
