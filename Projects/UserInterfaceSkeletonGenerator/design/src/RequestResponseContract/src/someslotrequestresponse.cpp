#include "someslotrequestresponse.h"

#include "businessobject.h"

SomeSlotRequestResponse::SomeSlotRequestResponse(BusinessObject *businessObject)
    : m_SomeSlotScopedResponder(new SomeSlotFinishedSignalEmitterOnDestruct(), this)
{
    QObject::connect(m_SomeSlotScopedResponder.data(), &SomeSlotFinishedSignalEmitterOnDestruct::signalToBeEmittedInDestructor, businessObject, &BusinessObject::someSlotFinished);
}
SomeSlotScopedResponder SomeSlotRequestResponse::assumeResponsibilityToRespond()
{
    SomeSlotScopedResponder ret = m_SomeSlotScopedResponder;

    //ref -= 1
    m_SomeSlotScopedResponder.reset(); //lol hacky but works

    return ret;
}
