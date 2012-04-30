#ifndef APPLOGICREQUEST_H
#define APPLOGICREQUEST_H

#include <QHash>
#include <QList>

#include "applogicrequestresponse.h"
#include "../abcapplogic.h"
#include "../../shared/WtFrontEndAndAppDbProtocol.h"

class AppLogicRequest
{
public:
    static AppLogicRequest *giveMeAnInheritedAppLogicRequestBasedOnTheMessage(WtFrontEndToAppDbMessage *message);
    static QHash<WtFrontEndToAppDbMessage::TheMessage, QList<AppLogicRequest*>* > m_RecycledAppLogicRequestsByTheMessage;
    static void returnAnInheritedAppLogicRequest(AppLogicRequest *appLogicRequest);
    static AbcAppLogic *m_AppLogic;
    static void setAppLogic(AbcAppLogic *appLogic);

    uint getRequestorId(); //i guess this method could still be helpful if we got it from out child object...
    BankServerActionRequest *bankServerActionRequest();
    bool bankServerRequestTriggered();

    //unsafe -- should only be called from AppLogic object/thread
    void processAppLogicRequest(); //process checks for race conditions, then calls perform
private:
    Q_DISABLE_COPY(AppLogicRequest)

    AppLogicRequest(WtFrontEndToAppDbMessage *networkRequestMessage);
    AppLogicRequestResponse *m_AppLogicRequestResponse;

    void setNetworkRequestMessage(WtFrontEndToAppDbMessage *networkRequestMessage);
    WtFrontEndToAppDbMessage *getNetworkRequestMessage();
    WtFrontEndToAppDbMessage *m_NetworkRequestMessage;

    //void setRequestorId(uint requestorId);
    //uint m_RequestorId;
    bool m_BankServerRequestTriggered;
    BankServerActionRequest *m_BankServerActionRequest;
protected:
    void setBankServerRequest(BankServerActionRequest *bankServerRequest);
    void setBankServerRequestTriggered(bool triggered);
    virtual bool raceCondition1detected()=0;
    virtual bool raceCondition2detected()=0;
    virtual void performAppLogicRequest()=0;
};

#endif // APPLOGICREQUEST_H
