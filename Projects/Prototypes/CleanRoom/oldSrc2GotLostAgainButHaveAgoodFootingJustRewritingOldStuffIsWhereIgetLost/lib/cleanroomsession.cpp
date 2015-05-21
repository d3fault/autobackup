#include "cleanroomsession.h"

#include "cleanroom.h"
#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

void CleanRoomSession::requestCleanRoomFrontPageDefaultView()
{
    QMetaObject::invokeMethod(m_CleanRoom, "getFrontPageDefaultViewBegin", Q_ARG(CleanRoomSession*, this));
}
void CleanRoomSession::requestCleanRoomApiCallOnceInCorrectContextAkaThread(ICrossDomainThreadSafeApiCall *crossDomainThreadSafeApiCall)
{
    QMetaObject::invokeMethod(m_CleanRoom, cleanRoomFrontPageDefaultViewRequestFromQt->apiCallSlot(), cleanRoomFrontPageDefaultViewRequestFromQt->optionalApiCallArgs());
}
