#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

#include <boost/bind.hpp>

CleanRoomFrontPageDefaultViewRequestFromWt::CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoomSession *session, const std::string &wtSessionId, boost::function<void (QVariantList)> wApplicationCallback)
    : IFrontPageDefaultViewRequest(session)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
void CleanRoomFrontPageDefaultViewRequestFromWt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    boost::bind(m_WApplicationCallback, responseArgs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, responseArgs));
}
