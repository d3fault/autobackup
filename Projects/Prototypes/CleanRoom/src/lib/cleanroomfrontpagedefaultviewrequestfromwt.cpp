#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

#include <boost/bind.hpp>

CleanRoomFrontPageDefaultViewRequestFromWt::CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (QVariantList)> wApplicationCallback)
    : IFrontPageDefaultViewRequest(cleanRoom)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
#if 0
void CleanRoomFrontPageDefaultViewRequestFromWt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    boost::bind(m_WApplicationCallback, responseArgs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, responseArgs));
}
#endif
void CleanRoomFrontPageDefaultViewRequestFromWt::respond(QStringList frontPageDocs)
{
    boost::bind(m_WApplicationCallback, frontPageDocs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, frontPageDocs));
}
