#include "someslotfinishedsignalemitterondestruct.h"

#include "someslotrequestresponse.h"

using namespace BusinessObjectRequestResponseContract;

//TODOoptimization: see if there's a way to let a STACK (only) object go out of scope to trigger the emit, rather than a HEAP allocated object like I'm currently using. probably is possible, but might need to refactor a ton. that stack deallocated object could [conditionally] call a pure virtual method [implementation], which then triggers the emit. or maybe it needn't be a virtual method at all (function callback?), I'm unsure. in ANY case, 'dereferencing a pointer and executing code at the address it points to' is much faster than 'allocating memory on the heap'. using ptrs like this just makes for easier [initial?] programming/thinking/idk-i-suck
SomeSlotFinishedSignalEmitterOnDestruct::SomeSlotFinishedSignalEmitterOnDestruct(bool emitOnDestruct)
    : QObject(nullptr)
    , m_EmitOnDestruct(emitOnDestruct)
    , m_Success(false)
    , m_XIsEven(false)
{ }
void SomeSlotFinishedSignalEmitterOnDestruct::setEmitOnDestruct(bool emitOnDestruct)
{
    m_EmitOnDestruct = emitOnDestruct;
}
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
    /*
    TO DOnereq: I don't think the signal that is emitted from destructor should ever go off when responding has been deferred... even when the app closes and the request has never been responded to, we should NOT then emit the signal in the destructor as the BusinessClass goes out of scope!

    the signal(s) should only go off in between assumeResponsibilityToRespond and deferResponsibility. this is a design decision and maybe not even the right one, but I'm making it. it sounds much worse to have a bunch of spurious signals go off right before the application exits. signals that might have been emitted eventually but were currently waiting for something to happen (io or timer or whatever). yes in fact it sounds preposterous now that I put it that way
    */
    if(m_EmitOnDestruct)
        emit signalToBeEmittedInDestructor(m_Success, m_XIsEven);
}

SomeSlotScopedResponder::SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent)
    : std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>(data)
    , m_Parent(parent)
{ }
void SomeSlotScopedResponder::deferResponding()
{
    m_Parent->m_SomeSlotScopedResponder = std::move(*this); //brain = exploded
    m_Parent->m_SomeSlotScopedResponder->setEmitOnDestruct(false);
}
void SomeSlotScopedResponder::reset(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent)
{
    std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>::reset(data);
    m_Parent = parent;
}
