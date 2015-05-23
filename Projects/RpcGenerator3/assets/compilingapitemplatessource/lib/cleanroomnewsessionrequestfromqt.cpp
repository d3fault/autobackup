#include "cleanroomnewsessionrequestfromqt.h"

void CleanRoomNewSessionRequestFromQt::respond(CleanRoomSession *session)
{
    emit newSessionResponseRequested(session);
}
