#ifndef SOMESLOTREQUESTRESPONSE_H
#define SOMESLOTREQUESTRESPONSE_H

#include "someslotfinishedsignalemitterondestruct.h"

class BusinessObject;

namespace BusinessObjectRequestResponseContract
{

class SomeSlotRequestResponse
{
public:
    SomeSlotRequestResponse(BusinessObject *businessObject);
    SomeSlotScopedResponder assumeResponsibilityToRespond();
private:
    BusinessObject *m_BusinessObject;

    SomeSlotScopedResponder m_SomeSlotScopedResponder;
    friend class SomeSlotScopedResponder;
};

}

#endif // SOMESLOTREQUESTRESPONSE_H
