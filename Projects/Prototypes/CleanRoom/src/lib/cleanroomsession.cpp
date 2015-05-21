#include "cleanroomsession.h"

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"
#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot)
{
    CleanRoomFrontPageDefaultViewRequestFromQt *request = new CleanRoomFrontPageDefaultViewRequestFromQt(this, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QVariantList)> wApplicationCallback)
{
    CleanRoomFrontPageDefaultViewRequestFromWt *request = new CleanRoomFrontPageDefaultViewRequestFromWt(this, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
