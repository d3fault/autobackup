#ifndef ICLEANROOMNEWSESSIONREQUEST
#define ICLEANROOMNEWSESSIONREQUEST

#include <QObject>
#include "icleanroomrequest.h"

#include "icleanroom.h"
#include "cleanroomsession.h"

class ICleanRoomNewSessionRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomNewSessionRequest(ICleanRoom *cleanRoom, QObject *parent = 0)
        : QObject(parent)
    {
        connect(this, &ICleanRoomNewSessionRequest::newSessionRequested, cleanRoom, &ICleanRoom::newSession);
    }
    void processRequest()
    {
        emit newSessionRequested(this);
    }
    virtual void respond(CleanRoomSession newSession)=0;
signals:
    void newSessionRequested(ICleanRoomNewSessionRequest *newSessionRequest);
};

#endif // ICLEANROOMNEWSESSIONREQUEST
