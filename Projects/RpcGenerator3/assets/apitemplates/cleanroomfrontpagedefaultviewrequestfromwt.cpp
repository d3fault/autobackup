#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromwt.h"

#include <boost/bind.hpp>

%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt::%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt(%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback)
    : I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%(%API_AS_VARIABLE_NAME%)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
void %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt::respond(QStringList frontPageDocs)
{
    boost::bind(m_WApplicationCallback, frontPageDocs);
    //Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, frontPageDocs));
}
