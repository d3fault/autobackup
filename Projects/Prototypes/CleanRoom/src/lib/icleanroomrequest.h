#ifndef ICLEANROOMREQUEST_H
#define ICLEANROOMREQUEST_H

#include <QVariant>

class CleanRoomSession;

class ICleanRoomRequest
{
public:
    ICleanRoomRequest(CleanRoomSession *session)
        : m_Session(session)
        //, m_SlotThatHandlesRequest(slotThatHandlesRequest)
    {
        //QMetaObject::invokeMethod(session->cleanRoom(), slotToRequest);
    }
    void respond(QVariant responseArg0)
    {
        QVariantList responseArgs;
        responseArgs.append(responseArg0);
        regainContextPossiblyInOtherThread_aka_respondActual(responseArgs);
    }
    void respond(QVariant responseArg0, QVariant responseArg1) //etc up to ~10
    {
        QVariantList responseArgs;
        responseArgs.append(responseArg0);
        responseArgs.append(responseArg1);
        regainContextPossiblyInOtherThread_aka_respondActual(responseArgs);
    }
    virtual void invokeSlotThatHandlesRequest()=0;
    virtual void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)=0;
protected:
    CleanRoomSession *m_Session;
};

#endif // ICLEANROOMREQUEST_H
