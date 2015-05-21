#include "ifrontpagedefaultviewrequest.h"

void IFrontPageDefaultViewRequest::invokeSlotThatHandlesRequest()
{
    QMetaObject::invokeMethod(m_Session->cleanRoom(), "getFrontPageDefaultView" /* derp m_SlotThatHandlesRequest*//*, Q_ARG(QString, ifThereWasArequestParamThisWouldBeIt*/);
}
