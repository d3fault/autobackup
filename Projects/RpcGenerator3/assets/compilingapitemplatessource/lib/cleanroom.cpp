#include "cleanroom.h"

#include "icleanroomfrontpagedefaultviewrequest.h"

CleanRoom::CleanRoom(QObject *parent)
    : ICleanRoom(parent)
{ }
void CleanRoom::getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0)
{
    //eventually the business gathers the frontPageDocs and calls finishedGettingFrontPageDefaultView, but we synthesize/fake that here
    //this is a good starting point for AutoDrilldown
    QStringList frontPageDocs;
    frontPageDocs << (someArg0 == 69420 ? "woot 69420!" : "aww nope rekt not 69420");
    request->respond(frontPageDocs);
}
