#include "someslotrequestresponse.h"

#include "../../businessobject.h"

using namespace BusinessObjectRequestResponseContract;

SomeSlotRequestResponse::SomeSlotRequestResponse(BusinessObject *businessObject)
    : m_BusinessObject(businessObject)
    , m_SomeSlotScopedResponder(new SomeSlotFinishedSignalEmitterOnDestruct(false), this)
{ }
SomeSlotScopedResponder SomeSlotRequestResponse::assumeResponsibilityToRespond()
{
    //jit create if necessary
    if(!m_SomeSlotScopedResponder)
    {
        m_SomeSlotScopedResponder.reset(new SomeSlotFinishedSignalEmitterOnDestruct(false), this);
        QObject::connect(m_SomeSlotScopedResponder.get(), &SomeSlotFinishedSignalEmitterOnDestruct::signalToBeEmittedInDestructor, m_BusinessObject, &BusinessObject::someSlotFinished); //TODOoptimization: see if we can keep everything connect'd (and re-use same connections) in between multiple successive slot invocations
    }

    m_SomeSlotScopedResponder->setEmitOnDestruct(true);
    return std::move(m_SomeSlotScopedResponder);
}
