#ifndef BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
#define BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H

#include "someslotscopedresponder.h"

class BusinessObject;

class SomeSlotRequestResponse;

class BusinessObjectRequestResponseContracts
{
public:
    explicit BusinessObjectRequestResponseContracts(BusinessObject *businessClass);
    SomeSlotRequestResponse *someSlot() const;
private:
    BusinessObject *m_BusinessObject;

    SomeSlotRequestResponse *m_SomeSlot;
};

#endif // BUSINESSOBJECTREQUESTRESPONSECONTRACTS_H
