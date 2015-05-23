#ifndef %API_NAME_UPPERCASE%SESSION
#define %API_NAME_UPPERCASE%SESSION

//TODOreq: #include <QStringList>

#include <boost/function.hpp>

#include "i%API_NAME_LOWERCASE%request.h"

class QObject;

class %API_NAME%;

class %API_NAME%Session
{
public:
    %API_NAME%Session(%API_NAME% *%API_AS_VARIABLE_NAME%)
        : m_%API_NAME%(%API_AS_VARIABLE_NAME%)
    { }
    static void requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback);

%API_CALL_METHODS_ON_SESSION_OBJECT_HEADER%private:
    %API_NAME% *m_%API_NAME%;
    static void invokeRequest(I%API_NAME%Request *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // %API_NAME_UPPERCASE%SESSION

