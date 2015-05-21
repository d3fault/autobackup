#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const char *slotCallback)
    : ICleanRoomFrontPageDefaultViewRequest(cleanRoom)
{
    QObject::connect(this, SIGNAL(frontPageDefaultViewResponseRequested(QStringList)), objectCallback, slotCallback);
}
void CleanRoomFrontPageDefaultViewRequestFromQt::respond(QStringList frontPageDocs)
{
    emit frontPageDefaultViewResponseRequested(frontPageDocs);
}
