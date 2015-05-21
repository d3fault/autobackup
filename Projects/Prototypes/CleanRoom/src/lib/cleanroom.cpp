#include "cleanroom.h"

#include "icleanroomrequest.h"

CleanRoom::CleanRoom(QObject *parent)
    : QObject(parent)
{ }
void CleanRoom::getFrontPageDefaultViewBegin(/*TODOreq: args? like 'request' or 'response path' etc...*/)
{
    //NOTE: this prototype says "fuck the db". rationale: the business <--> db comm is another layer of api calling/communication that just kinda fucks with my head too much

    //eventually the db gives us the frontPageDocs: in finishedGettingFrontPageDefaultView, but we synthesize/fake that here
    QStringList frontPageDocs;
    frontPageDocs << "woot doc";
    finishedGettingFrontPageDefaultView();
}
void CleanRoom::finishedGettingFrontPageDefaultView(ICleanRoomRequest *request, QStringList frontPageDocs)
{
    //somehow, we propagate those frontPageDocs to the front-ends. tons of ways to do it, but I want minimal/elegant/KISS xD
    //some things to consider:
    //0) 'jumping back to' the front-end  context/thread
    //1) doing the front-end's frontPageDefaultView callback
    //2) variable amount of arguments in the callback. i want static type safety if i can, but would settle for qt meta type safety if i must

    //maybe OT: it sounds like good design to not have the CleanRoom 'business' object know about sessions at all. it just sounds more modular i suppose. maybe someone wants to use the business without instantiating a 'session'. idfk, maybe it's fine to have a 'session' be mandatory. still, 'typical' api usage usually does not involve/require the use of 'sessions'. the only reason my 'widgets gui' has a session is because my 'web gui' needs them

    request->respond(QVariant::fromValue(frontPageDocs));
}
