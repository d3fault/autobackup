#ifndef %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
#define %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H

%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%

#include <QStringList>

#include <boost/function.hpp>

class %API_NAME%;

class %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt : public I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%
{
public:
    %API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%FromWt(%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (QStringList/*frontPageDocs*/)> wApplicationCallback);
    void respond(QStringList frontPageDocs);
private:
    std::string m_WtSessionId;
    boost::function<void (QStringList/*frontPageDocs*/)> m_WApplicationCallback;
};

#endif // %API_NAME_UPPERCASE%FRONTPAGEDEFAULTVIEWREQUESTFROMWT_H
