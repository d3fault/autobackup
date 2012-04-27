#ifndef APPLOGICREQUEST_H
#define APPLOGICREQUEST_H

#include <QList>

#include "applogicrequestresponse.h"
#include "../abcapplogic.h"
#include "../../shared/WtFrontEndAndAppDbProtocol.h"

class AppLogicRequest
{
public:
    //static AppLogicRequest *fromWtFrontEndToAppDbMessage(WtFrontEndToAppDbMessage message);
    static QList<AppLogicRequest*> m_RecycledAppLogicRequests;
    static AppLogicRequest *giveMeAnAppLogicRequest();
    static void returnAnAppLogicRequest(AppLogicRequest *appLogicRequest);
    static AbcAppLogic *m_AppLogic;
    static void setAppLogic(AbcAppLogic *appLogic);
    void processAppLogicRequest();
private:
    Q_DISABLE_COPY(AppLogicRequest)

    AppLogicRequest();
    AppLogicRequestResponse *m_AppLogicRequestResponse;
    WtFrontEndToAppDbMessage *m_WtFrontEndToAppDbMessage;
};

#endif // APPLOGICREQUEST_H
