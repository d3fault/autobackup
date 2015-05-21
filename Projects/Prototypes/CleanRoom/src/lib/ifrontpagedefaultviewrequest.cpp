#include "ifrontpagedefaultviewrequest.h"

void IFrontPageDefaultViewRequest::invokeSlotThatHandlesRequest()
{
    QMetaObject::invokeMethod(m_CleanRoom, "getFrontPageDefaultView", Q_ARG(IFrontPageDefaultViewRequest*,this));
}
void IFrontPageDefaultViewRequest::respond(QStringList frontPageDocs)
{

}
