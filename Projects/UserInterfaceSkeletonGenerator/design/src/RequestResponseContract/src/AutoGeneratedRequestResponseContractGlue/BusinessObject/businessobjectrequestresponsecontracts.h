#ifndef BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
#define BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H

#include "someslotrequestresponse.h"
#include "someslotfinishedsignalemitterondestruct.h"

namespace BusinessObjectRequestResponseContract
{

class BusinessObjectRequestResponseContracts
{
public:
    BusinessObjectRequestResponseContracts(BusinessObject *businessObject);
    SomeSlotRequestResponse &someSlot();
    //TODOreq: AnotherSlotRequestResponse anotherSlot();
private:
    SomeSlotRequestResponse m_SomeSlotRequestResponse;
    //TODOreq: AnotherSlotRequestResponse m_AnotherSlotRequestResponse;
};

}

#endif // BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
