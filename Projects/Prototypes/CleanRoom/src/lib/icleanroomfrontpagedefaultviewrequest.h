#ifndef ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
#define ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "icleanroomrequest.h"

#include "cleanroom.h"

class ICleanRoomFrontPageDefaultViewRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomFrontPageDefaultViewRequest(CleanRoom *cleanRoom, QObject *parent = 0)
        : QObject(parent)
    {
        connect(this, &ICleanRoomFrontPageDefaultViewRequest::frontPageDefaultViewRequested, cleanRoom, &CleanRoom::getFrontPageDefaultView);
    }
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
signals:
    void frontPageDefaultViewRequested(ICleanRoomFrontPageDefaultViewRequest *request);
};

#endif // ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
