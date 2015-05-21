#include "ifrontpagedefaultviewrequest.h"

void IFrontPageDefaultViewRequest::invokeSlotThatHandlesRequest()
{
    QMetaObject::invokeMethod(m_CleanRoom, "getFrontPageDefaultView", Q_ARG(ICleanRoomRequest*,this));
}
