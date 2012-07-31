#include "imessage.h"

IMessage::IMessage(QObject *owner)
    : IRecycleableAndStreamable(owner)
{ }
void IMessage::deliver()
{
    emit deliverSignal();
}
void IMessage::fail(quint32 failedReasonEnum)
{
    this->Header.Success = false;
    this->FailedReasonEnum = failedReasonEnum;
}
