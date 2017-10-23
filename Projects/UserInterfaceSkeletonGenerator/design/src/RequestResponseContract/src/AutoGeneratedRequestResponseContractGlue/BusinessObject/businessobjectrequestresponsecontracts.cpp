#include "businessobjectrequestresponsecontracts.h"

using namespace BusinessObjectRequestResponseContract;

BusinessObjectRequestResponseContracts::BusinessObjectRequestResponseContracts(BusinessObject *businessObject)
    : m_SomeSlotRequestResponse(businessObject)
    //TODOreq: , m_AnotherSlotRequestResponse(businessObject)
{ }
SomeSlotRequestResponse &BusinessObjectRequestResponseContracts::someSlot()
{
    return m_SomeSlotRequestResponse;
}
//TODOreq: anotherSlot() impl, simply returning m_AnotherSlotRequestResponse
