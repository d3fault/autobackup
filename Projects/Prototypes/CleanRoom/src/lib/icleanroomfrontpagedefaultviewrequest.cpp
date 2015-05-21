#include "icleanroomfrontpagedefaultviewrequest.h"

void ICleanRoomFrontPageDefaultViewRequest::processRequest()
{
    emit frontPageDefaultViewRequested(this);
}
