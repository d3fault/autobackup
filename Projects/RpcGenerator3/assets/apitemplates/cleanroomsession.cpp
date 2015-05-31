%RPC_AUTO_GENERATED_FILE_WARNING%
#include "%API_NAME_LOWERCASE%session.h"

#include "%API_NAME_LOWERCASE%newsessionrequestfromqt.h"
#include "%API_NAME_LOWERCASE%newsessionrequestfromwt.h"
%API_CALL_REQUEST_HEADER_INCLUDES%

void %API_NAME%Session::requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot)
{
    %API_NAME%NewSessionRequestFromQt *request = new %API_NAME%NewSessionRequestFromQt(%API_AS_VARIABLE_NAME%, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void %API_NAME%Session::requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session)> wApplicationCallback)
{
    %API_NAME%NewSessionRequestFromWt *request = new %API_NAME%NewSessionRequestFromWt(%API_AS_VARIABLE_NAME%, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
%API_CALL_METHODS_ON_SESSION_OBJECT_SOURCE%
