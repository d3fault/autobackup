#ifndef CLEANROOMSESSION_H
#define CLEANROOMSESSION_H

#include <QByteArray>

class CleanRoom;
class ICrossDomainThreadSafeApiCall;
class ICleanRoomSessionRequest;
class ICleanRoomFrontEndResponder;

#include "frontpagedefaultviewcleanroomsessionrequest.h"

class CleanRoomSession
{
public:
    CleanRoomSession(CleanRoom *cleanRoom, ICleanRoomFrontEndResponder *responder)
        : m_CleanRoom(cleanRoom)
        , m_Responder(responder)
    { }
    void requestCleanRoomFrontPageDefaultView();
    void requestCleanRoomApiCallOnceInCorrectContextAkaThread(ICrossDomainThreadSafeApiCall *crossDomainThreadSafeApiCall);
protected:
    //API requests
    ICleanRoomSessionRequest *createFrontPageDefaultViewSessionRequest()
    {
        return new FrontPageDefaultViewCleanRoomSessionRequest(this);
    }
    ICleanRoomSessionRequest *createRegisterSessionRequest(const QString &registerUsername, const QString &registerPassword, const QString &callBack)
    {
        return 0; //TODOreq
        //return new RegisterSessionRequest(this);
    }
#if 0 //TODoreq:
    ICleanRoomSessionRequest *createLoginSessionRequest()
    {
        return new LoginSessionRequest(this);
    }
    ICleanRoomSessionRequest *createSubmitDataSessionRequest(QByteArray data)
    {
        return new SubmitDataSessionRequest(data, this);
    }
#endif
protected:
    CleanRoom *m_CleanRoom;
private:
    friend class ICleanRoomSessionRequest;
    ICleanRoomFrontEndResponder *m_Responder;
protected: //signals
    //virtual void sessionRequestRequested(ICleanRoomSessionRequest *request)=0;
public: //slots
    //void respond(ICleanRoomFrontEndResponder *response)=0;
};

#endif // CLEANROOMSESSION_H
