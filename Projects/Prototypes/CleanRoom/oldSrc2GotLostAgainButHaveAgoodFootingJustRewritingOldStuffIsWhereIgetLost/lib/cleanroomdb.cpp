#include "cleanroomdb.h"

CleanRoomDb::CleanRoomDb(QObject *parent)
    : QObject(parent)
{ }
void CleanRoomDb::getFrontPageDefaultViewBegin(CleanRoomSession *session)
{
    //business thread calls this directly, it is our responsibility to thread-safely call the db
    //TODOreq: this is where we cross thread call/notify the couchbase/db thread via libevent

    //eventually, the db thread safely calls us back at handleFinishedGettingFrontPageDefaultView
}
void CleanRoomDb::handleFinishedGettingFrontPageDefaultView(CleanRoomSession *session, QList<QString> frontPageEntries)
{
    //TODOreq: the backend does a thread safe method invoke to get back here

    //end then we use normal signals/slots communication to get back to the business (in this case, it's going to be a direct signal invocation, but we could use a regular callback since we know we're on the business thread)
    emit finishedGettingFrontPageDefaultView(session, frontPageEntries);
}
