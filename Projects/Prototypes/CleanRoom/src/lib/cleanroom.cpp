#include "cleanroom.h"

#include "icleanroomsession.h"
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

//Requests always get here via Qt event loop (invoke method etc)
//Responses get back to the requester's thread/context in a variety of ways
CleanRoom::CleanRoom(QObject *parent)
    : QObject(parent)
{
    //registerCleanRoomApiCall()
}
void CleanRoom::initializeAndStart()
{
    //typically, we'd async start/connect-to the db and then do this:
    emit readyForSessions();
}
void CleanRoom::handleNewSession(ICleanRoomSession *session)
{
    QObject *sessionAsQObject = static_cast<QObject*>(session);
    //connect(session, SIGNAL(frontPageViewQueryAndSubscribeRequested()))
    connect(session, SIGNAL(sessionRequestRequested(ICleanRoomSessionRequest*)), this, SLOT(handleSessionRequestRequested(ICleanRoomSessionRequest*)));

    //synthesize the front page default view session request immediately
    FrontPageDefaultViewCleanRoomSessionRequest *frontPageDefaultViewCleanRoomSessionRequest = new FrontPageDefaultViewCleanRoomSessionRequest(session);
    handleSessionRequestRequested(frontPageDefaultViewCleanRoomSessionRequest);
}
void CleanRoom::handleSessionRequestRequested(ICleanRoomSessionRequest *sessionRequest)
{
    //the purpose of this slot is to get into the right thread/context before calling processRequest
    sessionRequest->processRequest();
}
void CleanRoom::getFrontPageDefaultViewBegin(FrontPageDefaultViewCleanRoomSessionRequest *request)
{
    emit getFrontPageDefaultViewFromDbRequested(request);
}
void CleanRoom::handleFinishedGettingFrontPageDefaultViewFromDb(FrontPageDefaultViewCleanRoomSessionRequest *request, QList<QString> frontPageDocs)
{
    request->m_Session->m_Responder->respond(request);
    //response->respondToFrontEndSession();
}
