#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

CleanRoomFrontPageDefaultViewRequestFromWt::CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoomSession *session, boost::function<QStringList> wApplicationCallback)
    : m_Session(session)
    , m_WApplicationCallback(wApplicationCallback)
{ }
