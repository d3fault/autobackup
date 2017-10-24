#include "businessobjectrequestresponsecontracts.h"

#include "businessobject.h"

#include "someslotrequestresponse.h"
//TODOreq: #include "anothersotrequestresponse.h"

using namespace BusinessObjectRequestResponseContracts;

Contracts::Contracts(BusinessObject *businessObject)
    : m_SomeSlot(new SomeSlotRequestResponse(businessObject, businessObject))
    //TODOreq: , m_AnotherSlot(new AnotherSlotRequestResponse(businessObject, businessObject))
{ }
SomeSlotRequestResponse *Contracts::someSlot() const
{
    return m_SomeSlot;
}
//TODOreq: anotherSlot() impl, simply returning m_AnotherSlot
