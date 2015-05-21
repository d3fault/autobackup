#include "cleanroomguisessionresponder.h"

CleanRoomGuiSessionResponder::CleanRoomGuiSessionResponder(QObject *widgetAkaQObjectToRespondTo, const char *slotNameOnThatWidgetThatHandlesSessionResponses) //the Wt impl might take a WApplication pointer, sessionID string, and boost function pointer
{
    connect(this, SIGNAL(responseRequested(ICleanRoomSessionRequest*)), widgetAkaQObjectToRespondTo, slotNameOnThatWidgetThatHandlesSessionResponses);
}
void CleanRoomGuiSessionResponder::respond(ICleanRoomSessionRequest *request)
{
    //this method gets back into position of the context/thread of the requester (from the business context/thread), and then calls request->respond(), which uses a callback or function pointer or whatever to execute the method designated along with the request
    emit responseRequested(request); //the Wt impl calls WServer::post
}
