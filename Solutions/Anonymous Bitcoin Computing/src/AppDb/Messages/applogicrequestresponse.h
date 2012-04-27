#ifndef APPLOGICREQUESTRESPONSE_H
#define APPLOGICREQUESTRESPONSE_H

#include "applogicrequest.h"

class AppLogicRequestResponse
{
public:
    Q_DISABLE_COPY(AppLogicRequestResponse)

    AppLogicRequestResponse(AppLogicRequest *parentRequest);
    AppLogicRequest *parentRequest();
private:
    AppLogicRequest *m_ParentRequest;
};

#endif // APPLOGICREQUESTRESPONSE_H
