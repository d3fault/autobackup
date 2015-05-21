#include "cleanroom.h"

#include "cleanroomdb.h"
#include "cleanroomsession.h"
#include "frontpagedefaultviewcleanroomsessionrequest.h"

#if 0
struct CleanRoomApiCall
{
    QObject *metaObjectToCallApiOn;
    QMetaMethod metaMethodApiCall;
    QList<QVariant> metaMethodArgsForApiCall;
};
class CleanRoomApiRequestExpectingResponse
{
    CleanRoomApiCall apiCall;
    CleanRoomSessionResponder responder;
};
//yes that works, but when I call
responder.respond(); //I get back in my requester's context/thread but now what should I use to activate an ACTUAL callback for the specific api call?
#endif

//OT-ish: By dispatching the "default view" when a "new session", I am optimizing. One way for the front-end to still be able to efficiently/cleanly receive a pseudo-"session received" event would be for two slots to be connected to the "businessResponded" signal. The first one (and order matters) simply gobbles up the session pointer (which is a property of the first request/response (the default view)) and disconnects itself (a slot) from that signal. the second slot is our typical "handleBusinessResponded" that assumes our session pointer is valid. For now it will just be "known" by the coder whether or not there is a default dispatched view for a given session, but there are programmatic ways of accounting for that too. by disconnecting the 'first' slot from the signal, we take the code path of checking if the session pointer is equal to zero out of the picture. that's what i mean by efficiently/cleanly

//Requests always get here via Qt event loop (invoke method etc)
//Responses get back to the requester's thread/context in a variety of ways
CleanRoom::CleanRoom(QObject *parent)
    : QObject(parent)
    , m_Db(new CleanRoomDb(this))
{
    //registerCleanRoomApiCall()
    connect(m_Db, SIGNAL(finishedGettingFrontPageDefaultView(CleanRoomSession*,QList<QString>)), this, SLOT(handleFinishedGettingFrontPageDefaultViewFromDb(CleanRoomSession*,QList<QString>)));
}
void CleanRoom::initializeAndStart()
{
    //typically, we'd async start/connect-to the db and then do this:
    emit readyForSessions();
}
void CleanRoom::startNewSession(ICleanRoomFrontEndResponder *cleanRoomFrontEndResponder)
{
    QSharedPointer<CleanRoomSession> session(new CleanRoomSession(this, cleanRoomFrontEndResponder)); //TODOreq: delete when session ends (wt = when WApplication ends it's session, qt = when widget is closed)
    cleanRoomFrontEndResponder->respondSayingNewSessionSuccessful(session); //TODOoptimiation: deliver the 'default view' without them requesting it

    m_Sessions.append(session);

    /*
    QObject *sessionAsQObject = static_cast<QObject*>(session);
    //connect(session, SIGNAL(frontPageViewQueryAndSubscribeRequested()))
    connect(session, SIGNAL(sessionRequestRequested(ICleanRoomSessionRequest*)), this, SLOT(handleSessionRequestRequested(ICleanRoomSessionRequest*)));

    //synthesize the front page default view session request immediately
    FrontPageDefaultViewCleanRoomSessionRequest *frontPageDefaultViewCleanRoomSessionRequest = new FrontPageDefaultViewCleanRoomSessionRequest(session);
    handleSessionRequestRequested(frontPageDefaultViewCleanRoomSessionRequest);
    */
    //session->requestCleanRoomFrontPageDefaultView(/*no args to this api call*/); //usually the 'front end' calls it, but we call it here as an optimization (and out of laziness/simplicity). TODOreq the method call should invoke a meta method, so it should be a direct call right now and a queued invoke if it's a front end calling it
}
void CleanRoom::handleSessionRequestRequested(ICleanRoomSessionRequest *sessionRequest)
{
    //the purpose of this slot is so the caller can get into the right thread/context before processRequest is called
    sessionRequest->processRequest();
}
void CleanRoom::getFrontPageDefaultViewBegin(CleanRoomSession *session)
{
    //emit getFrontPageDefaultViewFromDbRequested(session);
    m_Db->getFrontPageDefaultViewBegin(session);
}
void CleanRoom::handleFinishedGettingFrontPageDefaultViewFromDb(CleanRoomSession *session, QList<QString> frontPageDocs)
{
    //request->m_Session->m_Responder->respond(request);
    //response->respondToFrontEndSession();

    //db got our response and we our now in the business thread on the business object. just need to propagate the value to the front-end
    //session->responder()
    session->responder()->respond(session, frontPageDocs); //shouldn't i specify that this is a front page request response? maybe the session already 'knows' (aka, one request at a time is enforced)
}
