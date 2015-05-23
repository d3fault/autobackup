#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include "icleanroomfrontpagedefaultviewrequest.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public ICleanRoomFrontPageDefaultViewRequest
{
    Q_OBJECT
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const char *slotCallback, double someArg0);
    void respond(QStringList frontPageDocs);
signals:
    void frontPageDefaultViewResponseRequested(QStringList frontPageDocs);
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
