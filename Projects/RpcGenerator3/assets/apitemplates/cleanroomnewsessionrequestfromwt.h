#ifndef %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMWT_H
#define %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMWT_H

#include "i%API_NAME_LOWERCASE%newsessionrequest.h"

#include <boost/function.hpp>

class %API_NAME%Session;

class %API_NAME%NewSessionRequestFromWt : public I%API_NAME%NewSessionRequest
{
public:
    %API_NAME%NewSessionRequestFromWt(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_NAME%Session*)> wApplicationCallback)
        : I%API_NAME%NewSessionRequest(%API_AS_VARIABLE_NAME%)
        , m_WtSessionId(wtSessionId)
        , m_WApplicationCallback(wApplicationCallback)
    { }
    void respond(%API_NAME%Session *session);
private:
    std::string m_WtSessionId;
    boost::function<void (%API_NAME%Session*)> m_WApplicationCallback;
};

#endif // %API_NAME_UPPERCASE%NEWSESSIONREQUESTFROMWT_H
