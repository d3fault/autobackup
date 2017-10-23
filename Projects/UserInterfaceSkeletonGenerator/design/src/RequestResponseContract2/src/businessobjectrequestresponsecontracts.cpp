#include "businessobjectrequestresponsecontracts.h"

#include "businessobject.h"
#include "someslotrequestresponse.h"

//this class doesn't need to be a QObject strictly speaking, but it's children/members do... so I might as well maintain any QObject heirarchy going on so that QObject::moveToThread functions as intended. oh nvm realized I'm passing in BusinessObject* xD wewt
BusinessObjectRequestResponseContracts::BusinessObjectRequestResponseContracts(BusinessObject *businessClass)
    : m_BusinessObject(businessClass)
    , m_SomeSlot(new SomeSlotRequestResponse(businessClass, businessClass))
{ }
SomeSlotRequestResponse *BusinessObjectRequestResponseContracts::someSlot() const
{
    return m_SomeSlot;
}
