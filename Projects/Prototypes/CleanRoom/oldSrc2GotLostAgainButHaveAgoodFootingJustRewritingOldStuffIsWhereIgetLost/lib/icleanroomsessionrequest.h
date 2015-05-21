#ifndef ICLEANROOMSESSIONREQUEST_H
#define ICLEANROOMSESSIONREQUEST_H

#include <QVariant>

class CleanRoom;
class CleanRoomSession;

class ICleanRoomSessionRequest
{
public:
    ICleanRoomSessionRequest(CleanRoomSession *session)
        : m_Session(session)
    { }
    virtual void processRequest()=0;
private:
    friend class ICleanRoomFrontEndResponder;
    CleanRoomSession *m_Session;

    CleanRoom *cleanRoom() const;
    QVariantList optionalResponseArguments;
};

#endif // ICLEANROOMSESSIONREQUEST_H
