#include "cleanroomguiresponder.h"

CleanRoomGuiResponder::CleanRoomGuiResponder(QObject *parentWidgetToRespondTo, const char *slotNameOnThatWidgetToHandleSessionGreeting, const char *slotNameOnThatWidgetThatHandlesSessionResponses) //the Wt impl might take a WApplication pointer, sessionID string, and boost function pointer
    : QObject(parentWidgetToRespondTo)
{
    connect(this, SIGNAL(newSessionResponseRequested(CleanRoomSession*)), parentWidgetToRespondTo, slotNameOnThatWidgetThatHandlesSessionResponses);
    connect(this, SIGNAL(responseRequested(ICleanRoomSessionRequest*)), parentWidgetToRespondTo, slotNameOnThatWidgetThatHandlesSessionResponses);
}
void CleanRoomGuiResponder::respondSayingNewSessionSuccessful(CleanRoomSession *session)
{
    emit newSessionResponseRequested(session); //the Wt impl calls WServer::post
}
void CleanRoomGuiResponder::respond(ICleanRoomSessionRequest *request)
{
    //this method gets back into position of the context/thread of the requester (from the business context/thread), and then calls request->respond(), which uses a callback or function pointer or whatever to execute the method designated along with the request
    emit responseRequested(request); //the Wt impl calls WServer::post
}
