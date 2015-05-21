#include "icrossdomainthreadsafeapicall.h"

ICrossDomainThreadSafeApiCall::ICrossDomainThreadSafeApiCall(QObject *targetObjectToCallApiOn, const char *apiSlotToCall)
    : m_TargetObjectToCallApiOn(targetObjectToCallApiOn)
    , m_ApiSlotToCall(apiSlotToCall)
{ }
const char *ICrossDomainThreadSafeApiCall::apiSlotToCall() const
{
    return m_ApiSlotToCall;
}
