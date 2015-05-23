#include "icleanroomfrontpagedefaultviewrequest.h"

void ICleanRoomFrontPageDefaultViewRequest::processRequest()
{
    emit frontPageDefaultViewRequested(this, m_SomeArg0);
}
