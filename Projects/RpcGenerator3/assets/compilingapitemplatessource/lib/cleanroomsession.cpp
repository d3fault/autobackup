#include <QVariant>

#include "cleanroomsession.h"

#include "cleanroomnewsessionrequestfromqt.h"
#include "cleanroomnewsessionrequestfromwt.h"

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"
#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

void CleanRoomSession::requestNewSession(ICleanRoom *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot)
{
    CleanRoomNewSessionRequestFromQt *request = new CleanRoomNewSessionRequestFromQt(cleanRoom, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void CleanRoomSession::requestNewSession(ICleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession)> wApplicationCallback)
{
    CleanRoomNewSessionRequestFromWt *request = new CleanRoomNewSessionRequestFromWt(cleanRoom, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot, double someArg0) const
{
    CleanRoomFrontPageDefaultViewRequestFromQt *request = new CleanRoomFrontPageDefaultViewRequestFromQt(cleanRoom(), *this, objectToCallbackTo, callbackSlot, someArg0);
    invokeRequest(request);
}
void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback, double someArg0) const
{
    CleanRoomFrontPageDefaultViewRequestFromWt *request = new CleanRoomFrontPageDefaultViewRequestFromWt(cleanRoom(), *this, wtSessionId, wApplicationCallback, someArg0);
    invokeRequest(request);
}
