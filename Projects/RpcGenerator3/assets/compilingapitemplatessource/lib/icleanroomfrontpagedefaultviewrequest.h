#ifndef ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
#define ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "icleanroomrequest.h"

#include "cleanroomsession.h"

class ICleanRoom;

class ICleanRoomFrontPageDefaultViewRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomFrontPageDefaultViewRequest(ICleanRoom *cleanRoom, CleanRoomSession parentSession, double someArg0, QObject *parent = 0);
    CleanRoomSession parentSession() const;
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
private:
    CleanRoomSession m_ParentSession;

    double m_SomeArg0;
signals:
    void frontPageDefaultViewRequested(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
};

#endif // ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
