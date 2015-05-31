#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "icleanroomfrontpagedefaultviewrequest.h"

class CleanRoomFrontPageDefaultViewRequestFromQt : public ICleanRoomFrontPageDefaultViewRequest
{
    Q_OBJECT
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(ICleanRoom *cleanRoom, CleanRoomSession parentSession, QObject *objectCallback, const char *slotCallback, double someArg0);
    void respond(QStringList frontPageDocs);
signals:
    void frontPageDefaultViewResponseRequested(QStringList frontPageDocs);
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
