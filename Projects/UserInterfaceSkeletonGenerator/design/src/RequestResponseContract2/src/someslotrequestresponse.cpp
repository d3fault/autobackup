#include "someslotrequestresponse.h"

#include "businessobject.h"

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
    m_XIsEven = xIsEven;
}
void SomeSlotRequestResponse::respond_aka_emitFinishedSignal()
{
    emit someSlotFinished(m_Success, m_XIsEven);
    m_Success = false; //after emit we set back to false for the NEXT slot invocation
}
