#ifndef %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
#define %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H

#include "i%API_NAME_LOWERCASE%frontpagedefaultviewrequest.h"

#include <QStringList>

#include <boost/function.hpp>

class %API_NAME%;

class %API_NAME%FrontPageDefaultViewRequestFromWt : public I%API_NAME%FrontPageDefaultViewRequest
{
public:
    %API_NAME%FrontPageDefaultViewRequestFromWt(%API_NAME% *cleanRoom, const std::string &wtSessionId, boost::function<void (QStringList/*frontPageDocs*/)> wApplicationCallback);
    void respond(QStringList frontPageDocs);
private:
    std::string m_WtSessionId;
    boost::function<void (QStringList/*frontPageDocs*/)> m_WApplicationCallback;
};

#endif // %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
