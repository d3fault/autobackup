#include "someslotfinishedsignalemitterondestruct.h"

#include "someslotrequestresponse.h"

SomeSlotFinishedSignalEmitterOnDestruct::SomeSlotFinishedSignalEmitterOnDestruct()
    : QObject(nullptr)
    , m_Success(false)
{ }
void SomeSlotFinishedSignalEmitterOnDestruct::setSuccess(bool success)
{
    m_Success = success;
}
SomeSlotFinishedSignalEmitterOnDestruct::~SomeSlotFinishedSignalEmitterOnDestruct()
{
    emit signalToBeEmittedInDestructor(m_Success);
}

SomeSlotScopedResponder::SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent)
    : std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>(data)
    , m_Parent(parent)
{ }
void SomeSlotScopedResponder::deferResponding()
{
    m_Parent->m_SomeSlotScopedResponder = std::move(*this); //brain = exploded
}
