#ifndef ICLEANROOMSESSIONRESPONDER
#define ICLEANROOMSESSIONRESPONDER

class ICleanRoomSessionRequest;

//TODOreq: the responder's sole purpose is to get back into the requester's context/thread, and then call request->respond once there
class ICleanRoomSessionResponder
{
public:
    void respond(ICleanRoomSessionRequest *request)=0;
    /*{
        //this method exists so that db->business calls are thread safe
        QMetaObject::invokeMethod(m_Request->m_Session, "respond", Q_ARG(ICleanRoomSessionResponder, this));
    }*/
};

#endif // ICLEANROOMSESSIONRESPONDER

