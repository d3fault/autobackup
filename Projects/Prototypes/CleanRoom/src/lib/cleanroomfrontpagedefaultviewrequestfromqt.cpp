#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, const char *slotCallback)
    : m_Session(session)
    , m_SlotCallback(slotCallback)
{ }
