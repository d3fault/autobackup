#include "cleanroom.h"

#include "cleanroomsession.h"
#include "icleanroomnewsessionrequest.h"
#include "icleanroomfrontpagedefaultviewrequest.h"

CleanRoom::CleanRoom(QObject *parent)
    : QObject(parent)
{ }
void CleanRoom::initializeAndStart()
{
    //wait for business to become ready etc, then:
    emit readyForSessions();
}
void CleanRoom::newSession(ICleanRoomNewSessionRequest *request)
{
    request->respond(new CleanRoomSession(this));
}
void CleanRoom::getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0)
{
    //eventually the business gathers the frontPageDocs and calls finishedGettingFrontPageDefaultView, but we synthesize/fake that here
    QStringList frontPageDocs;
    frontPageDocs << (someArg0 == 69420 ? "woot 69420!" : "aww nope rekt not 69420");
    finishedGettingFrontPageDefaultView(request, frontPageDocs);
}
void CleanRoom::finishedGettingFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, QStringList frontPageDocs)
{
    request->respond(frontPageDocs);
}
