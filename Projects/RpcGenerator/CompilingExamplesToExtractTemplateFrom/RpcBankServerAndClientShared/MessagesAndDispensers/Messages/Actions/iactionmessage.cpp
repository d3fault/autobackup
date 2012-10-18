#include "iactionmessage.h"

IActionMessage::IActionMessage(QObject *parent) :
    IMessage(parent)
{ }

void IActionMessage::setSuccessful()
{
    m_Successful = true;
}
bool IActionMessage::isSuccessful()
{
    return m_Successful;
}
quint8 IActionMessage::errorCode()
{
    return m_ErrorCode;
}
void IActionMessage::resetSuccessAndErrorCode()
{
    m_Successful = false;
    m_ErrorCode = 0;
}
void IActionMessage::setErrorCode(quint8 errorCode)
{
    //m_Success = false; //TO DONEreq: this shouldn't be required as Success should be set to false on dispensing etc. It doesn't seem like that bad of an idea, however... just to be safe. Note that DEPENDING on it is a terrible idea, because then we can't handle the Generic Error case safely/consistently. for that reason i'd say leave it out
    m_ErrorCode = errorCode;
}

#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
void IActionMessage::streamSuccessAndErrorCodeIn(in)
{
    in >> Success;
    in >> ErrorCode;
}
#endif

#ifdef WE_ARE_RPC_BANK_SERVER_CODE
void IActionMessage::streamSuccessAndErrorCodeOut(QDataStream &out)
{
    out << Success;
    out << ErrorCode;
}
#endif
