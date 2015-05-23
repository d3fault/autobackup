#ifndef ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
#define ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "icleanroomrequest.h"

class ICleanRoom;

class ICleanRoomFrontPageDefaultViewRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomFrontPageDefaultViewRequest(ICleanRoom *cleanRoom, double someArg0, QObject *parent = 0);
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
private:
    double m_SomeArg0;
signals:
    void frontPageDefaultViewRequested(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
};

#endif // ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
