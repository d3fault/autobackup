#ifndef APPLOGICREQUEST_H
#define APPLOGICREQUEST_H

#include <QList>

#include "applogicrequestresponse.h"
#include "../abcapplogic.h"
#include "../../shared/WtFrontEndAndAppDbProtocol.h"

class AppLogicRequest
{
public:
    static QList<AppLogicRequest*> m_RecycledAppLogicRequests;
    static AppLogicRequest *giveMeAnAppLogicRequest(uint requestorId);
    static void returnAnAppLogicRequest(AppLogicRequest *appLogicRequest);
    static AbcAppLogic *m_AppLogic;
    static void setAppLogic(AbcAppLogic *appLogic);

    uint getRequestorId();

    //unsafe -- should only be called from AppLogic object/thread
    void processAppLogicRequest();
private:
    Q_DISABLE_COPY(AppLogicRequest)

    AppLogicRequest();
    AppLogicRequestResponse *m_AppLogicRequestResponse;
    WtFrontEndToAppDbMessage *m_WtFrontEndToAppDbMessage;

    void setRequestorId(uint requestorId);
    uint m_RequestorId;
};

#endif // APPLOGICREQUEST_H
