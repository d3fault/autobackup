#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

#include <boost/bind.hpp>

CleanRoomFrontPageDefaultViewRequestFromWt::CleanRoomFrontPageDefaultViewRequestFromWt(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback, double someArg0)
    : ICleanRoomFrontPageDefaultViewRequest(cleanRoom, someArg0)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
void CleanRoomFrontPageDefaultViewRequestFromWt::respond(QStringList frontPageDocs)
{
    boost::bind(m_WApplicationCallback, frontPageDocs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, frontPageDocs));
}
