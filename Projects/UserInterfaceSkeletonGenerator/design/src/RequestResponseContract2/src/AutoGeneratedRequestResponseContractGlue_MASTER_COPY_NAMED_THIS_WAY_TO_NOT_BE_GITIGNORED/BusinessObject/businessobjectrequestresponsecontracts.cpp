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
    return m_SomeSlot; //TODOmb: instead of returning a pointer to the SomeSlotRequestResponse, we could return the ScopedResponder pre-instantiated. I always thought that "methods returning a local var" returned a COPY of the local var (and that local var is destructed (and in our case, signal emitted (which results in the desired signal being emitted twice!))), but my testing actually showed otherwise, even when NOT compiling against C++11. It's some compiler optimization, that's the thing: I need a COMPILE TIME GUARANTEE that the object will not be destructed. I looked into std::move and move constructors stuff and did some testing, but actually the only version that worked (no double destruct) was the version that did NOT use std::move etc... so wtf? I don't think it's very smart to hinge FUNCTIONALITY on some compile time optimization that, under certain circumstances, might NOT be used. I need a compile time guarantee!!! fkn unique_ptr can give me one, why can't a regular stack var? If I get that compile time guarantee, users of this code could simply write: auto scopedResponder = m_Contracts.someSlot(); //rather than: BusinessClassSomeSlotScopedResponder scopedResponder(m_Contracts); -- I recently realized that my namespace strategy isn't good with QObject/Q_OBJECT stuff :(, so need to add business class back into the name :(
}
//TODOreq: anotherSlot() impl, simply returning m_AnotherSlot
