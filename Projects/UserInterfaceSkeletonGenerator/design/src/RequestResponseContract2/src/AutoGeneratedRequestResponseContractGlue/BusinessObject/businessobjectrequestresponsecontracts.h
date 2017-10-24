#ifndef BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
#define BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H

#include "someslotscopedresponder.h"

class BusinessObject;

namespace BusinessObjectRequestResponseContracts
{

class SomeSlotRequestResponse;

class Contracts
{
public:
    explicit Contracts(BusinessObject *businessObject);
    SomeSlotRequestResponse *someSlot() const;
    //TODOreq: AnotherSlotRequestResponse *anotherSlot() const;
private:
    SomeSlotRequestResponse *m_SomeSlot;
    //TODOreq: AnotherSlotRequestResponse *m_AnotherSlot;
};

}

#endif // BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
