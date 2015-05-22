#ifndef %API_NAME_UPPERCASE%SESSION
#define %API_NAME_UPPERCASE%SESSION

#include <QStringList>

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
    %API_NAME% *%API_AS_VARIABLE_NAME%() const
    {
        return m_%API_NAME%;
    }
    static void requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback);
    void requestNew%API_NAME%FrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot);
    void requestNew%API_NAME%FrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QStringList)> wApplicationCallback);
private:
    %API_NAME% *m_%API_NAME%;
    static void invokeRequest(I%API_NAME%Request *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // %API_NAME_UPPERCASE%SESSION

