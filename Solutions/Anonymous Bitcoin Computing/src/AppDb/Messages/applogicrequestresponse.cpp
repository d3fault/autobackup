#include "applogicrequestresponse.h"

AppLogicRequestResponse::AppLogicRequestResponse(AppLogicRequest *parentRequest)
{
    m_ParentRequest = parentRequest;
}
AppLogicRequest * AppLogicRequestResponse::parentRequest()
{
    return m_ParentRequest;
}
