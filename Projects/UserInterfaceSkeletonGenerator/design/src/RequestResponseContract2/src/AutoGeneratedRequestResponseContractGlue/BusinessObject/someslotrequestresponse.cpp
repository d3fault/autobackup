#include "someslotrequestresponse.h"

#include "businessobject.h"

using namespace BusinessObjectRequestResponseContracts;

SomeSlotRequestResponse::SomeSlotRequestResponse(BusinessObject *businessClass, QObject *parent)
    : QObject(parent)
    , m_Success(false)
{
    connect(this, &SomeSlotRequestResponse::someSlotFinished, businessClass, &BusinessObject::someSlotFinished, Qt::QueuedConnection);
}
void SomeSlotRequestResponse::setSuccess(bool success)
{
    m_Success = success;
}
void SomeSlotRequestResponse::setXIsEven(bool xIsEven)
{
    m_XIsEven = xIsEven; //TODOreq: default value mandatory! because they will likely want to let the scopedEmitter go out of scope, emitting the signal, without customizing any values. I think this requirement would be enforced in UserInterfaceSkeletonGenerator though, I don't see how it applies to this. I guess yea the only thing we'd see added is the constructor initializer of m_XIsEven using that required default value. just to clarify: slot args do NOT have default values (they must ALL be supplied at the time of calling), but signal args ALL have default values (so the slot can just "assumeResponsibilityToRespond()" and then call "return" without doing.... ANYTHING... at all (without specifying default values THEN)). alternatively I could just return uninitialized values, hmmm... yee old flexibility vs safety problem, oh rite this is C++ I'm supposed to choose flexibility (return uninitialized values), bbbuuut that doesn't mean I'm gonna, idk yet
}
void SomeSlotRequestResponse::respond_aka_emitFinishedSignal()
{
    emit someSlotFinished(m_Success, m_XIsEven);
    m_Success = false; //after emit we set back to false for the NEXT slot invocation
}
