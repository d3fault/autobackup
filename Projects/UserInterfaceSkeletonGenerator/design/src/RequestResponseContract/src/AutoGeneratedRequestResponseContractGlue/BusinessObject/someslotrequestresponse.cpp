#include "someslotrequestresponse.h"

#include "../../businessobject.h"

using namespace BusinessObjectRequestResponseContract;

SomeSlotRequestResponse::SomeSlotRequestResponse(BusinessObject *businessObject)
    : m_SomeSlotScopedResponder(new SomeSlotFinishedSignalEmitterOnDestruct(), this)
{
    QObject::connect(m_SomeSlotScopedResponder.get(), &SomeSlotFinishedSignalEmitterOnDestruct::signalToBeEmittedInDestructor, businessObject, &BusinessObject::someSlotFinished);
}
SomeSlotScopedResponder SomeSlotRequestResponse::assumeResponsibilityToRespond()
{
    return std::move(m_SomeSlotScopedResponder);
}
