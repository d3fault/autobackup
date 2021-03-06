#ifndef SOMESLOTSCOPEDRESPONDER_H
#define SOMESLOTSCOPEDRESPONDER_H

#include "someslotrequestresponse.h"

namespace BusinessObjectRequestResponseContracts
{

class SomeSlotScopedResponder
{
public:
    SomeSlotScopedResponder(SomeSlotRequestResponse *response);
    SomeSlotRequestResponse *response();
    void deferResponding();
    void resumeAutoRespondingDuringScopedDestruction();
    ~SomeSlotScopedResponder();
private:
    SomeSlotRequestResponse *m_SomeSlotRequestResponse;
    bool m_DeferResponding;
};

}

#endif // SOMESLOTSCOPEDRESPONDER_H
