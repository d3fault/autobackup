#include "%API_NAME_LOWERCASE%session.h"

#include "%API_NAME_LOWERCASE%newsessionrequestfromqt.h"
#include "%API_NAME_LOWERCASE%newsessionrequestfromwt.h"

#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromqt.h"
#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromwt.h"

void %API_NAME%Session::requestNewSession(%API_NAME% *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot)
{
    %API_NAME%NewSessionRequestFromQt *request = new %API_NAME%NewSessionRequestFromQt(cleanRoom, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void %API_NAME%Session::requestNewSession(%API_NAME% *cleanRoom, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback)
{
    %API_NAME%NewSessionRequestFromWt *request = new %API_NAME%NewSessionRequestFromWt(cleanRoom, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
void %API_NAME%Session::requestNew%API_NAME%FrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot)
{
    %API_NAME%FrontPageDefaultViewRequestFromQt *request = new %API_NAME%FrontPageDefaultViewRequestFromQt(m_%API_NAME%, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void %API_NAME%Session::requestNew%API_NAME%FrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback)
{
    %API_NAME%FrontPageDefaultViewRequestFromWt *request = new %API_NAME%FrontPageDefaultViewRequestFromWt(m_%API_NAME%, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
