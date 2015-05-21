#ifndef ICLEANROOMFRONTENDRESPONDER
#define ICLEANROOMFRONTENDRESPONDER

#include <QMetaType>

class CleanRoomSession;
class ICleanRoomSessionRequest;

//TODOreq: the responder's sole purpose is to get back into the requester's context/thread, and then call request->respond once there
class ICleanRoomFrontEndResponder
{
public:
    virtual void respondSayingNewSessionSuccessful(CleanRoomSession *session)=0;
    virtual void respond(ICleanRoomSessionRequest *request)=0;
};

#endif // ICLEANROOMFRONTENDRESPONDER

