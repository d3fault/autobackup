#include "ifrontpagedefaultviewrequest.h"

void IFrontPageDefaultViewRequest::invokeSlotThatHandlesRequest()
{
    emit frontPageDefaultViewRequested(this);
    //QMetaObject::invokeMethod(m_CleanRoom, "getFrontPageDefaultView", Q_ARG(IFrontPageDefaultViewRequest*,this));
}
#if 0
void IFrontPageDefaultViewRequest::respond(QStringList frontPageDocs)
{

}
#endif
