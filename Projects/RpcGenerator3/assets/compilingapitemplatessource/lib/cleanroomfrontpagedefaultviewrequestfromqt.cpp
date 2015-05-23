#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

//TODOreq: the QObject request impl can (so should?) accept a QObject *parent = 0 arg in constructor. the reason I didn't do it is so that it's made known that lifetime of the requests must be managed. because with the Wt ones, they are. I'm considering making the requests themselves COW (simplifies lifetime management), but unsure that's compatible with boost::function
CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const char *slotCallback, double someArg0)
    : ICleanRoomFrontPageDefaultViewRequest(cleanRoom, someArg0)
{
    QObject::connect(this, SIGNAL(frontPageDefaultViewResponseRequested(QStringList)), objectCallback, slotCallback); //TODOmb: I'm considering, as an optimization, having the "session" object be the one with the signals and slots. Not sure it's compatible with boost::function, just want to KISS and get this working first... but if I'm right then it would be more efficient because there wouldn't need to be a new connection connected/disconnected for each and every request. TODOoptimization: maybe the session should even lazy-connect for each request type (but maybe not, more logic in each and every request!)
}
void CleanRoomFrontPageDefaultViewRequestFromQt::respond(QStringList frontPageDocs)
{
    emit frontPageDefaultViewResponseRequested(frontPageDocs);
}
