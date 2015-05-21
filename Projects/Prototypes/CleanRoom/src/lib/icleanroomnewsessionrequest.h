#ifndef ICLEANROOMNEWSESSIONREQUEST
#define ICLEANROOMNEWSESSIONREQUEST

#include <QObject>
#include "icleanroomrequest.h"

#include "cleanroom.h"

class CleanRoomSession;

class ICleanRoomNewSessionRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomNewSessionRequest(CleanRoom *cleanRoom, QObject *parent = 0)
        : QObject(parent)
        , ICleanRoomRequest(cleanRoom)
    {
        connect(this, &ICleanRoomNewSessionRequest::newSessionRequested, cleanRoom, &CleanRoom::newSession); //TODOreq:later: Qt5 syntax signals/slots connections for compile time checking
    }
    void invokeSlotThatHandlesRequest()
    {
        emit newSessionRequested(this);
        //QMetaObject::invokeMethod(m_CleanRoom, "newSession", Q_ARG(ICleanRoomNewSessionRequest*, this));
    }
    virtual void respond(CleanRoomSession *newSession)=0;
#if 0
    {
        QMetaObject::invokeMethod(m_)
    }
#endif
signals:
    void newSessionRequested(ICleanRoomNewSessionRequest *newSessionRequest);
};

#endif // ICLEANROOMNEWSESSIONREQUEST
