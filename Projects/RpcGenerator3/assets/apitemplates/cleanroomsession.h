%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef %API_NAME_UPPERCASE%SESSION
#define %API_NAME_UPPERCASE%SESSION

#include <boost/function.hpp>

#include <QSharedData>

%ALL_API_CALLS_ADDITIONAL_INCLUDES_FOR_REQUEST_AND_RESPONSE_ARGS%

#include "i%API_NAME_LOWERCASE%request.h"

class QObject;

class I%API_NAME%;

class %API_NAME%SessionData : public QSharedData
{
public:
    %API_NAME%SessionData(I%API_NAME% *%API_AS_VARIABLE_NAME%)
        : m_%API_NAME%(%API_AS_VARIABLE_NAME%)%SESSIONDATA_CONSTRUCTOR_ADDITIONAL_SESSION_PROPERTIES_CONSTRUCTOR_INITIALIZERS%
    { }
    %API_NAME%SessionData(const %API_NAME%SessionData &other)
        : QSharedData(other)
        , m_%API_NAME%(other.m_%API_NAME%)%SESSIONDATA_CONSTRUCTOR_ADDITIONAL_SESSION_PROPERTIES_COPY_CONSTRUCTOR_INITIALIZERS%
    { }
    ~%API_NAME%SessionData() { }

    I%API_NAME% *m_%API_NAME%;%SESSIONDATA_ADDITIONAL_SESSION_PROPERTIES_MEMBER_DECLARATIONS%
};

class %API_NAME%Session
{
private:
    friend class I%API_NAME%;
    %API_NAME%Session(I%API_NAME% *%API_AS_VARIABLE_NAME%)
    {
        d = new %API_NAME%SessionData(%API_AS_VARIABLE_NAME%);
    }
public:
    %API_NAME%Session(const %API_NAME%Session &other)
        : d(other.d)
    { }

    I%API_NAME% *%API_AS_VARIABLE_NAME%() const { return d->m_%API_NAME%; }%SESSION_ADDITIONAL_SESSION_PROPERTIES_GETTERS_SETTERS_MEMBER_DECLARATIONS_AND_DEFINITIONS%

    static void requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, QObject *objectToCallbackTo, const char *callbackSlot);
    static void requestNewSession(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session)> wApplicationCallback);

%API_CALL_METHODS_ON_SESSION_OBJECT_HEADER%private:
    QSharedDataPointer<%API_NAME%SessionData> d;

    static void invokeRequest(I%API_NAME%Request *requestToInvoke)
    {
        requestToInvoke->processRequest();
    }
};

#endif // %API_NAME_UPPERCASE%SESSION

