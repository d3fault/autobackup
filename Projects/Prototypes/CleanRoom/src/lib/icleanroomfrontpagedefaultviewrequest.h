#ifndef ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
#define ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H

#include <QVariant>

class CleanRoomSession;

class ICleanRoomFrontPageDefaultViewRequest
{
public:
    ICleanRoomFrontPageDefaultViewRequest(CleanRoomSession *session)
        : m_Session(session)
    { }
    void respond(QVariant responseArg0)
    {
        QVariantList responseArgs;
        responseArgs.append(responseArg0);
        respondActual(responseArgs);
    }
    void respond(QVariant responseArg0, QVariant responseArg1) //etc up to ~10
    {
        QVariantList responseArgs;
        responseArgs.append(responseArg0);
        responseArgs.append(responseArg1);
        respondActual(responseArgs);
    }
    virtual void respondActual(QVariantList responseArgs)=0;
protected:
    CleanRoomSession *m_Session;
};

#endif // ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
