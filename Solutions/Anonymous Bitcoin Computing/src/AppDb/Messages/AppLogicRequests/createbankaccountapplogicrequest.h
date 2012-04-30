#ifndef CREATEBANKACCOUNTAPPLOGICREQUEST_H
#define CREATEBANKACCOUNTAPPLOGICREQUEST_H

class AppLogicRequest;
class WtFrontEndToAppDbMessage;

class CreateBankAccountAppLogicRequest : public AppLogicRequest
{
public:
    CreateBankAccountAppLogicRequest(WtFrontEndToAppDbMessage *networkRequestMessage);
protected:
    bool raceCondition1detected();
    bool raceCondition2detected();
    void performAppLogicRequest();
};

#endif // CREATEBANKACCOUNTAPPLOGICREQUEST_H
