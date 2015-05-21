#ifndef ICLEANROOMSESSION_H
#define ICLEANROOMSESSION_H

#include <QByteArray>

class CleanRoom;
class ICleanRoomSessionRequest;
class ICleanRoomSessionResponder;

class ICleanRoomSession
{
public:
    ICleanRoomSession(CleanRoom *cleanRoom, ICleanRoomSessionResponder *responder)
        : m_CleanRoom(cleanRoom)
        , m_Responder(responder)
    { }
protected:
    //API requests
    ICleanRoomSessionRequest *createFrontPageDefaultViewSessionRequest()
    {
        return new FrontPageDefaultViewSessionRequest(this);
    }
    ICleanRoomSessionRequest *createRegisterSessionRequest()
    {
        return new RegisterSessionRequest(this);
    }
    ICleanRoomSessionRequest *createLoginSessionRequest()
    {
        return new LoginSessionRequest(this);
    }
    ICleanRoomSessionRequest *createSubmitDataSessionRequest(QByteArray data)
    {
        return new SubmitDataSessionRequest(data, this);
    }
private:
    friend class ICleanRoomSessionRequest;
    CleanRoom *m_CleanRoom;
    ICleanRoomSessionResponder *m_Responder;
protected: //signals
    virtual void sessionRequestRequested(ICleanRoomSessionRequest *request)=0;
public: //slots
    void respond(ICleanRoomSessionResponder *response)=0;
};

#endif // ICLEANROOMSESSION_H

