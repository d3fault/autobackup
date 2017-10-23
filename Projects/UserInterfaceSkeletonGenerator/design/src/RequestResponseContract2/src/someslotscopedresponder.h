#ifndef SOMESLOTSCOPEDRESPONDER_H
#define SOMESLOTSCOPEDRESPONDER_H

#include "someslotrequestresponse.h"

class SomeSlotScopedResponder
{
public:
    SomeSlotScopedResponder(SomeSlotRequestResponse *response);
    SomeSlotRequestResponse *response();
    void deferResponding();
    void resumeResponding();
    ~SomeSlotScopedResponder();
private:
    SomeSlotRequestResponse *m_SomeSlotRequestResponse;
    bool m_DeferResponding;
};

#endif // SOMESLOTSCOPEDRESPONDER_H
