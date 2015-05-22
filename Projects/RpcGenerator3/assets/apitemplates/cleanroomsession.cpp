#include "%API_NAME_LOWERCASE%session.h"

#include "%API_NAME_LOWERCASE%newsessionrequestfromqt.h"
#include "%API_NAME_LOWERCASE%newsessionrequestfromwt.h"

#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromqt.h"
#include "%API_NAME_LOWERCASE%frontpagedefaultviewrequestfromwt.h"

void %API_NAME%Session::requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot)
{
    %API_NAME%NewSessionRequestFromQt *request = new %API_NAME%NewSessionRequestFromQt(%API_AS_VARIABLE_NAME%, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void %API_NAME%Session::requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback)
{
    %API_NAME%NewSessionRequestFromWt *request = new %API_NAME%NewSessionRequestFromWt(%API_AS_VARIABLE_NAME%, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
void %API_NAME%Session::requestNew%API_NAME%FrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot)
{
    %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromQt *request = new %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromQt(m_%API_NAME%, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void %API_NAME%Session::requestNew%API_NAME%FrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback)
{
    %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt *request = new %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt(m_%API_NAME%, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
