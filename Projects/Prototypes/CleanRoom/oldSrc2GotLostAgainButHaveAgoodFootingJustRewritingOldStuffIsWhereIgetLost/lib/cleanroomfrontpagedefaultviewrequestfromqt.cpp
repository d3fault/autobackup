#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

#include "cleanroomsession.h"

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, const char *slotCallback)
    : QtOriginatingCrossDomainThreadSafeApiCall(session, slotCallback)
    , CleanRoomFrontPageDefaultViewRequest()
{
    //session->requestCleanRoomFrontPageDefaultView();
    session->requestCleanRoomApiCallOnceInCorrectContextAkaThread(this);
}
