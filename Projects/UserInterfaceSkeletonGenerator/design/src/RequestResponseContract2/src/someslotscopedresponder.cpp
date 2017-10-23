#include "someslotscopedresponder.h"

SomeSlotScopedResponder::SomeSlotScopedResponder(SomeSlotRequestResponse *someSlotRequestResponse)
    : m_SomeSlotRequestResponse(someSlotRequestResponse)
    , m_DeferResponding(false)
{ }
SomeSlotRequestResponse *SomeSlotScopedResponder::response()
{
    return m_SomeSlotRequestResponse;
}
void SomeSlotScopedResponder::deferResponding()
{
    m_DeferResponding = true;
}
void SomeSlotScopedResponder::resumeResponding()
{
    m_DeferResponding = false;
}
SomeSlotScopedResponder::~SomeSlotScopedResponder()
{
    if(!m_DeferResponding)
        m_SomeSlotRequestResponse->respond_aka_emitFinishedSignal();
}
