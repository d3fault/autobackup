#include "someslotfinishedsignalemitterondestruct.h"

#include "someslotrequestresponse.h"

using namespace BusinessObjectRequestResponseContract;

SomeSlotFinishedSignalEmitterOnDestruct::SomeSlotFinishedSignalEmitterOnDestruct()
    : QObject(nullptr)
    , m_Success(false)
    , m_XIsEven(false)
{ }
void SomeSlotFinishedSignalEmitterOnDestruct::setSuccess(bool success)
{
    m_Success = success;
}
void SomeSlotFinishedSignalEmitterOnDestruct::setXIsEven(bool xIsEven)
{
    m_XIsEven = xIsEven; //TODOreq: default value mandatory! because they will likely want to let the scopedEmitter go out of scope, emitting the signal, without customizing any values. I think this requirement would be enforced in UserInterfaceSkeletonGenerator though, I don't see how it applies to this. I guess yea the only thing we'd see added is the constructor initializer of m_XIsEven using that required default value. just to clarify: slot args do NOT have default values (they must ALL be supplied at the time of calling), but signal args ALL have default values (so the slot can just "assumeResponsibilityToRespond()" and then call "return" without doing.... ANYTHING... at all (without specifying default values THEN)). alternatively I could just return uninitialized values, hmmm... yee old flexibility vs safety problem, oh rite this is C++ I'm supposed to choose flexibility (return uninitialized values), bbbuuut that doesn't mean I'm gonna, idk yet
}
SomeSlotFinishedSignalEmitterOnDestruct::~SomeSlotFinishedSignalEmitterOnDestruct()
{
    emit signalToBeEmittedInDestructor(m_Success, m_XIsEven);
}

SomeSlotScopedResponder::SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent)
    : std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>(data)
    , m_Parent(parent)
{ }
void SomeSlotScopedResponder::deferResponding()
{
    m_Parent->m_SomeSlotScopedResponder = std::move(*this); //brain = exploded
}
