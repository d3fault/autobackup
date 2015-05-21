#include "qtoriginatingcrossdomainthreadsafeapicall.h"

QtOriginatingCrossDomainThreadSafeApiCall::QtOriginatingCrossDomainThreadSafeApiCall(CleanRoomSession *session, const char *slotCallback)
    : ICrossDomainThreadSafeApiCall()
    , m_Session(session)
    , m_SlotCallback(slotCallback)
{ }
void QtOriginatingCrossDomainThreadSafeApiCall::returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters)
{
    //m_Session->
    QMetaObject::invokeMethod(static_cast<QObject*>(m_ObjectCallingApi), slotNameStdStr.c_str(), Q_ARG(QVariantList, optionalResponseParameters));
}
