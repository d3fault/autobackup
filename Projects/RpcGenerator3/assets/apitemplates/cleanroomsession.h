%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef %API_NAME_UPPERCASE%SESSION
#define %API_NAME_UPPERCASE%SESSION

#include <boost/function.hpp>

%ALL_API_CALLS_ADDITIONAL_INCLUDES_FOR_REQUEST_AND_RESPONSE_ARGS%

#include "i%API_NAME_LOWERCASE%request.h"

class QObject;

class I%API_NAME%;

class %API_NAME%Session
{
public:
    %API_NAME%Session(I%API_NAME% *%API_AS_VARIABLE_NAME%)
        : m_%API_NAME%(%API_AS_VARIABLE_NAME%)
    { }
    static void requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback);

%API_CALL_METHODS_ON_SESSION_OBJECT_HEADER%private:
    I%API_NAME% *m_%API_NAME%;
    static void invokeRequest(I%API_NAME%Request *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // %API_NAME_UPPERCASE%SESSION

